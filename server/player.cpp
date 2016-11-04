
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include <vector>
#include <iostream>

#include "player.hpp"
#include "zombie.hpp"
#include "../world.hpp"
#include "weapon.hpp"
#include "../packetid.hpp"
#include "gamestate.hpp"

unsigned short player_t::PLAYER_ID = 0;

player_t::player_t(gamestate_t *gamestate, float x, float y, std::string username){
	this->id = player_t::PLAYER_ID++;
	this->x = x;
	this->y = y;
	this->username = username;
	this->gamestate = gamestate;
	this->weapon = Weapon::shotgun;

	direction = 1;
}

void player_t::update(float delta){
	if(keyState[sf::Keyboard::A])
		vx = -moveSpeed;
	if(keyState[sf::Keyboard::D])
		vx = moveSpeed;

	if(!keyState[sf::Keyboard::A] && !keyState[sf::Keyboard::D]) vx *= 0.75;

	vy += gravity * delta;

	bool onGround = !gamestate->world->placeFree(x, y + vy*delta + 0.05);

	if(!onGround)
		y += vy*delta;
	else
		vy = 0;
	if(gamestate->world->placeFree(x + vx*delta, y))
		x += vx*delta;
	else
		vx = 0;

	if(onGround && keyState[sf::Keyboard::Space]){
		vy = -450; // jump_velocity = sqrt(2 * gravity * maximum_height)
	}

	if(mouseState[sf::Mouse::Left] && canShoot <= 0.0){
		shoot();
	}
	if(canShoot > 0.0) canShoot -= delta;
}

void player_t::shoot(){
	canShoot = weapon.shootDelay;

	zombie_t *zed = new zombie_t(x, y, gamestate->world);
	gamestate->zombies->push_back(zed);

	sf::Packet spawnZombiePacket;
	spawnZombiePacket << PACKET_ADD_ZOMBIE << (int) 1 << zed->id << zed->x << zed->y;
	send(spawnZombiePacket);
}

void player_t::setAddress(sf::UdpSocket *socket, sf::IpAddress addr, unsigned short port){
	this->addr = addr;
	this->port = port;
	this->socket = socket;
}

void player_t::send(sf::Packet packet){
	this->socket->send(packet, this->addr, this->port);
}
