
#include <SFML/Network.hpp>
#include "player.hpp"

player_t::player_t(unsigned short id, float x, float y, std::string username){
	this->id = id;
	this->x = x;
	this->y = y;
	this->username = username;
	
	direction = 1;
}

void player_t::setAddress(sf::UdpSocket *socket, sf::IpAddress addr, unsigned short port){
	this->addr = addr;
	this->port = port;
	this->socket = socket;
}

void player_t::sendPacket(sf::Packet packet){
	this->socket->send(packet, this->addr, this->port);
}
