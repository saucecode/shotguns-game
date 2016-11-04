#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

#include "../packetid.hpp"
#include "player.hpp"
#include "zombie.hpp"
#include "../world.hpp"

unsigned short PLAYER_PACKET_ID = 3;
unsigned short ZOMBIE_PACKET_ID = 0;


std::vector<zombie_t*> zombies;
std::vector<player_t*> players;
sf::UdpSocket socket;
unsigned short SERVER_PORT = 43234;
world_t *world;
sf::Packet worldDataPacket;


void createZombies(std::vector<zombie_t*>& zombies);
void gameLoop();
void networking();
bool setupServerSocket();

player_t* getPlayerByAddress(sf::IpAddress addr, unsigned short port, int *indexOf=nullptr);
player_t* getPlayerByID(unsigned short id);
void sendToAllExcept(sf::Packet packet, unsigned short id);
void sendToAll(sf::Packet packet);

int main(){
	std::cout << "shotguns static server  2016-10-08\n";

	if( !setupServerSocket() ) return 1;


	world = new world_t();
	world->addElement(new solid_t{-300,100,600,32});

	// forge PACKET_WORLD_DATA
	worldDataPacket << PACKET_WORLD_DATA;
	worldDataPacket << (int) world->elements.size();
	for(solid_t *solid : world->elements){
		worldDataPacket << solid->x << solid->y << solid->width << solid->height;
	}

	// create zombies, print them
	createZombies(zombies);
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
				packetPosition << PACKET_MOVE_PLAYER << (unsigned short) -1 << player->x << player->y;
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
			unsigned char packetid;
			sf::IpAddress client;
			unsigned short clientPort;
			socket.receive(packet, client, clientPort);
			packet >> packetid;

			if(packetid == PACKET_CONNECT){
				std::string username;
				packet >> username;
				std::cout << "Got connect request from " << client << " with name " << username << "\n";

				sf::Packet response;

				response << PACKET_CONNECT << true;
				player_t *newplayer = new player_t(PLAYER_PACKET_ID++, 0,0, username, world);
				newplayer->setAddress(&socket, client, clientPort);
				players.push_back(newplayer);
				newplayer->send(response);

				sf::Packet addPlayerPacket;
				addPlayerPacket << PACKET_ADD_PLAYER << newplayer->id << username;
				sendToAllExcept(addPlayerPacket, newplayer->id);

				for(player_t *player : players){
					if(player == newplayer) continue;
					sf::Packet addPlayerX;
					addPlayerX << PACKET_ADD_PLAYER << player->id << player->username;
					newplayer->send(addPlayerX);
				}

				newplayer->send(worldDataPacket);


				sf::Packet spawnZombiePacket;
				spawnZombiePacket << PACKET_ADD_ZOMBIE << (int) zombies.size();

				for(zombie_t *zed : zombies){
					spawnZombiePacket << zed->id << zed->x << zed->y;
				}

				newplayer->send(spawnZombiePacket);

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
				sf::Int8 direction;
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
				forceResponsePacket << PACKET_MOVE_PLAYER << (unsigned short) -1 << player->x << player->y;
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

			}
		}
	}
}

void sendToAll(sf::Packet packet){
	for(player_t *player : players){
		player->send(packet);
	}
}

void sendToAllExcept(sf::Packet packet, unsigned short id){
	for(player_t *player : players){
		if(player->id == id) continue;
		player->send(packet);
	}
}

player_t* getPlayerByID(unsigned short id){
	for(player_t *player : players){
		if(player->id == id)
			return player;
	}

	return nullptr;
}

player_t* getPlayerByAddress(sf::IpAddress addr, unsigned short port, int *indexOf){
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

void createZombies(std::vector<zombie_t*>& zombies){
	for(int i=0; i<8; i++){
		zombies.push_back(new zombie_t(ZOMBIE_PACKET_ID++, i*20.0f, -40, world));
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
