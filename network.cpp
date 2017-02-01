
#include <SFML/Network.hpp>

#include <string>
#include <thread>
#include <iostream>
#include <exception>
#include <cstdint>

#include "network.hpp"
#include "packetid.hpp"
#include "player.hpp"
#include "world.hpp"
#include "zombie.hpp"
#include "game.hpp"

#include "server/weapon.hpp"

Network::Network(Game *game, sf::IpAddress addr, unsigned short port){
	this->game = game;
	this->host = addr;
	this->port = port;
}

bool Network::connect(std::string username) {
	std::cout << "Connecting to server " << host << " with username " << username << "\n";

	sf::Packet connectionRequestPacket;
	connectionRequestPacket << PACKET_CONNECT << username;
	this->socket.send(connectionRequestPacket, host, port);

	sf::Packet response;
	sf::IpAddress sender;
	uint16_t senderport;
	uint8_t packetid;
	this->socket.receive(response, sender, senderport);

	if(sender != host || port != senderport){
		throw std::runtime_error("Got response from a different host.");
	}

	response >> packetid;

	if(packetid == PACKET_CONNECT){
		bool success;
		response >> success;
		return success;
	}else{
		throw std::runtime_error("Got non-standard response packet id.");
	}

	throw std::runtime_error("Reached bottom of connect function.");
	return false;
}

void Network::send(sf::Packet packet){
	socket.send(packet, host, port);
	TX += packet.getDataSize();
	LAST_TX += packet.getDataSize();
}

void Network::run(){
	sf::SocketSelector selector;
	selector.add(socket);

	while(running){
		if(selector.wait()){
			sf::Packet packet;
			sf::IpAddress packetAddr;
			uint16_t packetPort = 0;
			socket.receive(packet, packetAddr, packetPort);

			if(packetAddr != host || port != packetPort){
				std::cout << "Got rogue packet from " << packetAddr << ":" << packetPort << "\n";
				continue;
			}

			RX += packet.getDataSize();
			LAST_RX += packet.getDataSize();

			uint8_t packetid;
			packet >> packetid;

			if(packetid == PACKET_ADD_PLAYER){
				int16_t id = 0;
				std::string name;
				float x, y;

				packet >> id >> name >> x >> y;

				game->agents.push_back(new player_t(game, id, x, y, name));
				std::cout << "Added player " << id << " named " << name << "\n";

			}else if(packetid == PACKET_DROP_PLAYER){
				int16_t targetid;
				packet >> targetid;
				std::cout << "Received remove player packet for id " << targetid << "\n";
				int i=0;
				for(player_t *p : game->agents){
					if(p->id == targetid){
						game->agents.erase(game->agents.begin() + i);
						break;
					}
					i++;
				}

			}else if(packetid == PACKET_MOVE_PLAYER){
				int16_t playerid;
				float x,y;
				packet >> playerid >> x >> y;

				if(playerid == -1){
					game->player->x = x;
					game->player->y = y;
				}else{
					player_t *target = getPlayerByID(playerid);
					if(target != nullptr){
						target->x = x;
						target->y = y;
					}
				}

			}else if(packetid == PACKET_WORLD_DATA){
				if(!game->world->isLoaded){
					int solidCount;
					packet >> solidCount;
					for(int i=0; i<solidCount; i++){
						float x,y,w,h;
						packet >> x >> y >> w >> h;
						game->world->addElement(new solid_t{x,y,w,h});
					}
					std::cout << "Added " << solidCount << " new solids.\n";

					game->world->isLoaded = true;

				}

				sf::Packet acknowledge;
				acknowledge << PACKET_WORLD_DATA;
				send(acknowledge);

			}else if(packetid == PACKET_ADD_ZOMBIE){
				int count;
				packet >> count;
				for(int i=0; i<count; i++){
					uint16_t id;
					float x, y;
					packet >> id >> x >> y;
					game->zombies.push_back(new zombie_t(game, id, x, y));
				}
				std::cout << "Spawned " << count << " zombies (Total " << game->zombies.size() << ")\n";

			}else if(packetid == PACKET_MOVE_ZOMBIE){
				uint16_t id;
				float x, y;

				packet >> id >> x >> y;
				zombie_t *zed = getZombieByID(id);
				if(zed != nullptr){
					zed->x = x;
					zed->y = y;
				}

			}else if(packetid == PACKET_DROP_ZOMBIE){
				uint16_t zedID;
				packet >> zedID;
				zombie_t *zed = getZombieByID(zedID);
				if(zed != nullptr){
					zed->health = 0; // zed's dead
					std::cout << "Killed zombie ID " << zedID << "\n";
				}


			}else if(packetid == PACKET_PINGAZ){
				uint32_t counter;
				packet >> counter >> this->latency;

				send(packet);
				// std::cout << "Got ping - determined latency is " << this->latency * 1000 << "ms\n";

			}else if(packetid == PACKET_SPAWN_PROJECTILE){
				sf::Vector2f start, end;
				int16_t ownerid;
				packet >> ownerid >> start.x >> start.y >> end.x >> end.y;
				projectile_t *projectile = new projectile_t(ownerid, start, end);


				projectile->line[0].color = sf::Color(127,127,127);
				projectile->line[1].color = sf::Color(127,127,127);

				// std::cout << "received projectile ranged " << projectile->range << " id:" << ownerid << std::endl;

				game->projectilesMutex.lock();
				game->projectiles.push_back(projectile);
				game->projectilesMutex.unlock();

				// std::cout << "spawned projectile\n";

			}else if(packetid == PACKET_WHO_AM_I){
				int16_t myid;
				packet >> myid;
				game->player->id = myid;
				std::cout << "WHO_AM_I: id " << myid << "\n";

			}else if(packetid == PACKET_PLAYER_HEALTH_BLOB){
				while(!packet.endOfPacket()){
					int16_t targetID, targetHealth;
					packet >> targetID >> targetHealth;
					player_t *target = getPlayerByID(targetID);
					target->health = targetHealth;
				}
			}

		}
	}
}

player_t* Network::getPlayerByID(int16_t id){
	if(id == game->player->id) return game->player;
	for(player_t *player : game->agents){
		if(player->id == id)
			return player;
	}

	return nullptr;
}

zombie_t* Network::getZombieByID(uint16_t id){
	for(zombie_t *zed : game->zombies){
		if(zed->id == id)
			return zed;
	}

	return nullptr;
}


void Network::stop(){
	// breaks selector.wait() so that the thread can exit.
	sf::Packet stopPacket;
	stopPacket << sf::Uint8(100);
	running = false;
	this->socket.send(stopPacket, sf::IpAddress("127.0.0.1"), socket.getLocalPort());
}

void Network::disconnect(){
	sf::Packet packet;
	packet << PACKET_DISCONNECT;
	socket.send(packet, host, port);
}
