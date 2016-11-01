
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include "player.hpp"
#include "../world.hpp"

player_t::player_t(unsigned short id, float x, float y, std::string username,
		world_t *world){
	this->id = id;
	this->x = x;
	this->y = y;
	this->username = username;
	this->world = world;

	direction = 1;
}

void player_t::update(float delta){
	if(keyState[sf::Keyboard::A])
		vx = -moveSpeed * delta;
	if(keyState[sf::Keyboard::D])
		vx = moveSpeed * delta;

	if(!keyState[sf::Keyboard::A] && !keyState[sf::Keyboard::D]) vx *= 0.75;

	vy += gravity * delta;

	bool onGround = !world->placeFree(x, y + vy);

	if(!onGround)
		y += vy;
	else
		vy = 0;
	if(world->placeFree(x + vx, y))
		x += vx;
	else
		vx = 0;

	if(onGround && keyState[sf::Keyboard::Space]){
		vy = -10;
	}
}

void player_t::setAddress(sf::UdpSocket *socket, sf::IpAddress addr, unsigned short port){
	this->addr = addr;
	this->port = port;
	this->socket = socket;
}

void player_t::send(sf::Packet packet){
	this->socket->send(packet, this->addr, this->port);
}
