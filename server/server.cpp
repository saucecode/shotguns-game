#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cstdint>

#include "../packetid.hpp"
#include "player.hpp"
#include "zombie.hpp"
#include "../world.hpp"
#include "gamestate.hpp"

std::vector<zombie_t*> zombies;
std::vector<player_t*> players;
sf::UdpSocket socket;
uint16_t SERVER_PORT = 43234;
world_t *world;
sf::Packet worldDataPacket;
gamestate_t *gamestate;

void createZombies(std::vector<zombie_t*>& zombies, gamestate_t *gamestate);
void gameLoop();
void networking();
bool setupServerSocket();

player_t* getPlayerByAddress(sf::IpAddress addr, uint16_t port, int32_t *indexOf=nullptr);
player_t* getPlayerByID(int16_t id);
void sendToAllExcept(sf::Packet packet, int16_t id);
void sendToAll(sf::Packet packet);

int main(){
	std::cout << "shotguns static server  2016-10-08\n";

	if( !setupServerSocket() ) return 1;


	world = new world_t("samplemap.txt");

	/*
	world = new world_t();
	world->addElement(new solid_t{-300,100,600,32});
	world->addElement(new solid_t{-550,100,150,32});
	world->addElement(new solid_t{-550,16,150,16});
	*/
	// forge PACKET_WORLD_DATA
	worldDataPacket << PACKET_WORLD_DATA;
	worldDataPacket << (int) world->elements.size();
	for(solid_t *solid : world->elements){
		worldDataPacket << solid->x << solid->y << solid->width << solid->height;
	}

	gamestate = new gamestate_t;
	gamestate->world = world;
	gamestate->players = &players;
	gamestate->zombies = &zombies;

	// create zombies, print them
	createZombies(zombies, gamestate);
	for(zombie_t *zed : zombies){
		std::cout << zed->x << " " << zed->y << "\n";
	}

	// start networking thread
	std::thread networkThread(networking);

	gameLoop();

	networkThread.join();
	socket.unbind();
	return 0;
}

void gameLoop(){
	std::cout << "Started logic thread loop.\n";
	sf::Clock deltaTimer;
	float delta, seconds = 0;
	bool secondPassed = false; // value is true for one frame every second

	while(true){
		delta = deltaTimer.restart().asSeconds();
		seconds += delta;
		if(seconds >= 1.0){
			secondPassed = true;
			seconds = 0;
		}else{
			secondPassed = false;
		}

		for(player_t *player : players){
			player->update(delta);

			if(!player->hasDownloadedWorld && secondPassed)
				player->send(worldDataPacket);


			if(player->x != player->lastSentX || player->y != player->lastSentY){
				sf::Packet packetPosition;
				packetPosition << PACKET_MOVE_PLAYER << (uint16_t) -1 << player->x << player->y;
				player->send(packetPosition);

				sf::Packet positionPacket2;
				positionPacket2 << PACKET_MOVE_PLAYER << player->id << player->x << player->y;
				sendToAllExcept(positionPacket2, player->id);

				player->lastSentX = player->x;
				player->lastSentY = player->y;
			}

			// send PINGAZ packet every second
			if(secondPassed){
				sf::Packet pingPacket;
				pingPacket << PACKET_PINGAZ << player->pingTicker++ << player->latency;
				player->pingClock.restart();
				player->send(pingPacket);
			}
		}

		for(zombie_t *zed : zombies){
			zed->update(delta);

			sf::Packet zombiePosition;
			zombiePosition << PACKET_MOVE_ZOMBIE;
			zombiePosition << zed->id << zed->x << zed->y;
			sendToAll(zombiePosition);

			if(zed->health <= 0){
				sf::Packet killZombiePacket;
				killZombiePacket << PACKET_DROP_ZOMBIE << zed->id;
				sendToAll(killZombiePacket);
			}
		}

		zombies.erase(
			std::remove_if(zombies.begin(), zombies.end(),
				[](zombie_t *zed){
					return zed->health <= 0.0;
				}),
			zombies.end()
		);

		if(secondPassed){
			sf::Packet healthBlob;
			healthBlob << PACKET_PLAYER_HEALTH_BLOB;
			for(player_t *player : players){
				healthBlob << player->id << player->health;
			}
			sendToAll(healthBlob);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void networking(){
	std::cout << "Started networking thread.\n";

	sf::SocketSelector selector;
	selector.add(socket);

	while(true){
		if(selector.wait()){

			sf::Packet packet;
			uint8_t packetid;
			sf::IpAddress client;
			uint16_t clientPort;
			socket.receive(packet, client, clientPort);
			packet >> packetid;

			if(packetid == PACKET_CONNECT){
				std::string username;
				packet >> username;
				std::cout << "Got connect request from " << client << " with name " << username << "\n";

				sf::Packet response;

				response << PACKET_CONNECT << true;
				player_t *newplayer = new player_t(gamestate, 0, 0, username);
				newplayer->setAddress(&socket, client, clientPort);
				players.push_back(newplayer);
				newplayer->send(response);

				sf::Packet addPlayerPacket;
				addPlayerPacket << PACKET_ADD_PLAYER << newplayer->id << username;
				addPlayerPacket << newplayer->x << newplayer->y;
				sendToAllExcept(addPlayerPacket, newplayer->id);

				for(player_t *player : players){
					if(player == newplayer) continue;
					sf::Packet addPlayerX;
					addPlayerX << PACKET_ADD_PLAYER << player->id << player->username;
					addPlayerX << player->x << player->y;
					newplayer->send(addPlayerX);
				}

				newplayer->send(worldDataPacket);


				sf::Packet spawnZombiePacket;
				spawnZombiePacket << PACKET_ADD_ZOMBIE << (int) zombies.size();

				for(zombie_t *zed : zombies){
					spawnZombiePacket << zed->id << zed->x << zed->y;
				}

				newplayer->send(spawnZombiePacket);
				sf::Packet whoPacket;
				whoPacket << PACKET_WHO_AM_I << newplayer->id;
				newplayer->send(whoPacket);

			}else if(packetid == PACKET_DISCONNECT){
				std::cout << "Disconnect received.\n";
				int index;
				player_t *player = getPlayerByAddress(client, clientPort, &index);

				if(player != nullptr){
					sf::Packet disconnectPacket;
					disconnectPacket << PACKET_DROP_PLAYER << player->id;
					sendToAllExcept(disconnectPacket, player->id);
					std::cout << "Player " << player->id << " disconnected.\n";

					players.erase(players.begin() + index);
					delete player;
				}

			}else if(packetid == PACKET_MOVE_PLAYER){
				player_t *player = getPlayerByAddress(client, clientPort);
				int8_t direction;
				packet >> direction;

				if(direction == 'D'){
					//player->x += player->moveSpeed;
					player->keyState[sf::Keyboard::D] = true;
				}else if(direction == 'A'){
					player->x -= player->moveSpeed;
				}
				// std::cout << "Moved player " << player->id << " " << (int) magnitude << " units in " << direction << "\n";

				sf::Packet echo;
				echo << PACKET_MOVE_PLAYER << player->id << player->x << player->y;
				sendToAllExcept(echo, player->id);

				sf::Packet forceResponsePacket;
				forceResponsePacket << PACKET_MOVE_PLAYER << (int16_t) -1 << player->x << player->y;
				player->send(forceResponsePacket);

			}else if(packetid == PACKET_KEY_STATE){
				player_t *target = getPlayerByAddress(client, clientPort);

				// unpack key state snapshot blob to appropriate keys
				packet >> target->keyState[sf::Keyboard::W];
				packet >> target->keyState[sf::Keyboard::A];
				packet >> target->keyState[sf::Keyboard::S];
				packet >> target->keyState[sf::Keyboard::D];
				packet >> target->keyState[sf::Keyboard::Space];
				packet >> target->keyState[sf::Keyboard::Q];
				packet >> target->keyState[sf::Keyboard::E];
				packet >> target->keyState[sf::Keyboard::F];

				packet >> target->mouseState[0];
				packet >> target->mouseState[1];
				packet >> target->mousePosition[0];
				packet >> target->mousePosition[1];

			}else if(packetid == PACKET_WORLD_DATA){
				player_t *target = getPlayerByAddress(client, clientPort);
				target->hasDownloadedWorld = true;

			}else if(packetid == PACKET_PINGAZ){
				player_t *target = getPlayerByAddress(client, clientPort);
				int packetCount;
				packet >> packetCount;

				if(packetCount == target->pingTicker-1){
					target->latency = target->pingClock.restart().asSeconds();
					//std::cout << "Player " << target->id << " has ping " <<
					//	target->latency*1000 << "ms\n";
				}

			}else if(packetid == PACKET_WHO_AM_I){
				player_t *target = getPlayerByAddress(client, clientPort);
				packet << target->id;
				target->send(packet);

			}
		}
	}
}

void sendToAll(sf::Packet packet){
	for(player_t *player : players){
		player->send(packet);
	}
}

void sendToAllExcept(sf::Packet packet, int16_t id){
	for(player_t *player : players){
		if(player->id == id) continue;
		player->send(packet);
	}
}

player_t* getPlayerByID(int16_t id){
	for(player_t *player : players){
		if(player->id == id)
			return player;
	}

	return nullptr;
}

player_t* getPlayerByAddress(sf::IpAddress addr, uint16_t port, int *indexOf){
	int i=0;
	for(player_t *player : players){
		if(player->addr == addr && player->port == port){
			if(indexOf != nullptr) *indexOf = i;
			return player;
		}
		i++;
	}

	return nullptr;
}

void createZombies(std::vector<zombie_t*>& zombies, gamestate_t *gamestate){
	for(int i=0; i<2; i++){
		zombies.push_back(new zombie_t(gamestate, i*20.0f, -40));
	}
}

bool setupServerSocket(){
	std::cout << "Binding to port " << SERVER_PORT << "\n";
	if(socket.bind(SERVER_PORT) != sf::Socket::Done){
		std::cout << "Failed to bind to port.\n";
		return false;
	}
	return true;
}
