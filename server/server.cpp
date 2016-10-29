#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include "../packetid.hpp"
#include "player.hpp"
#include "../world.hpp"

unsigned short PLAYER_PACKET_ID = 3;
unsigned short ZOMBIE_PACKET_ID = 0;

typedef struct {
	unsigned short id;
	float x;
	float y;
	short health;
	char direction;
} zombie_t;


std::vector<zombie_t> zombies;
std::vector<player_t*> players;
sf::UdpSocket socket;
unsigned short SERVER_PORT = 43234;
world_t *world;


void createZombies(std::vector<zombie_t>& zombies);
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

	createZombies(zombies);

	for(zombie_t zed : zombies){
		std::cout << zed.x << " " << zed.y << "\n";
	}


	world = new world_t();
	world->addElement(new solid_t{-100,100,200,200});
	world->addElement(new solid_t{132,100,64,16});
	world->addElement(new solid_t{132+64,64,64,128});


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
	double delta;

	while(true){
		delta = deltaTimer.restart().asSeconds();

		for(player_t *player : players){
			player->update(delta);

			sf::Packet packetPosition;
			packetPosition << PACKET_MOVE_PLAYER << (unsigned short) -1 << player->x << player->y;
			player->send(packetPosition);

			sf::Packet positionPacket2;
			positionPacket2 << PACKET_MOVE_PLAYER << player->id << player->x << player->y;
			sendToAllExcept(positionPacket2, player->id);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(20));
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

				/*
				// MATRIX MODE!
				std::cout << target->keyState[sf::Keyboard::W] << " " <<
				target->keyState[sf::Keyboard::A] << " " <<
				target->keyState[sf::Keyboard::S] << " " <<
				target->keyState[sf::Keyboard::D] << " " <<
				target->keyState[sf::Keyboard::Space] << "\n";
				*/

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

void createZombies(std::vector<zombie_t>& zombies){
	for(int i=0; i<10; i++){
		zombie_t zed = {ZOMBIE_PACKET_ID++, i * 20.0f, 128, 20, 1};
		zombies.push_back(zed);
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
