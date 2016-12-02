
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include <vector>
#include <iostream>
#include <cmath>

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
	this->weapon = Weapon::p250;

	direction = 1;
}

void player_t::update(float delta){
	/*
			___  __   _____  _________   ________  ___
		   / _ \/ /  / _ \ \/ / __/ _ \ / ___/ _ \/ _ \
		  / ___/ /__/ __ |\  / _// , _// /__/ ___/ ___/
		 /_/  /____/_/ |_|/_/___/_/|_(_)___/_/  /_/
	*/
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

	//if(mouseState[sf::Mouse::Left] && canShoot <= 0.0){
	if(keyState[sf::Keyboard::E] && canShoot <= 0.0){
		deployZombie();
	}
	if(mouseState[0] && canShoot <= 0.0)
		shoot();
	if(canShoot > 0.0) canShoot -= delta;

	// CHEAT - return to origin
	if(keyState[sf::Keyboard::Q]){
		x = 0;
		y = 0;
		vx = 0;
		vy = 0;
	}
}

void player_t::shoot(){
	canShoot = weapon.shootDelay;
	float angle = atan2((float) mousePosition[1] - y + weaponHeight, (float) mousePosition[0] - x);

	projectile_t projectile = hitscan(gamestate->world, x, y - weaponHeight, angle, weapon.range);

	// TODO delete me - testing only
	sf::Packet projectilePacket;
	projectilePacket << PACKET_SPAWN_PROJECTILE;
	projectilePacket << projectile.start.x << projectile.start.y << projectile.end.x << projectile.end.y;

	for(auto *agent : *gamestate->players){
		agent->send(projectilePacket);
	}


	std::cout << "shot ranged at " << projectile.range << "\n";
}

projectile_t player_t::hitscan(world_t *world, float x, float y, float angle, const float range){
	float ix=x, iy=y;
	float dx = cos(angle) * 2;
	float dy = sin(angle) * 2;
	float distanceTravesedX = 0, distanceTravesedY = 0;

	while(distanceTravesedX*distanceTravesedX + distanceTravesedY*distanceTravesedY < range*range){
		if(!world->placeFree(x + dx, y + dy)) break;
		x += dx;
		y += dy;
		distanceTravesedX += dx;
		distanceTravesedY += dy;
	}

	projectile_t projectile(this, sf::Vector2f(ix,iy), sf::Vector2f(ix + distanceTravesedX, iy + distanceTravesedY));

	for(zombie_t *zed : *gamestate->zombies){
		if(player_t::lineIntersection(
			projectile.start, projectile.end,
			sf::Vector2f(zed->x, zed->y), sf::Vector2f(zed->x, zed->y-24)
		))
			zed->vy = -200;
	}

	return projectile;
}

bool player_t::lineIntersection(sf::Vector2f origin1, sf::Vector2f dest1,
		sf::Vector2f origin2, sf::Vector2f dest2){
	float s1_x = dest1.x - origin1.x;
	float s1_y = dest1.y - origin1.y;
	float s2_x = dest2.x - origin2.x;
	float s2_y = dest2.y - origin2.y;

	float s = (-s1_y * (origin1.x - origin2.x) + s1_x * (origin1.y - origin2.y)) / (-s2_x * s1_y + s1_x * s2_y);
	float t = ( s2_x * (origin1.y - origin2.y) - s2_y * (origin1.x - origin2.x)) / (-s2_x * s1_y + s1_x * s2_y);

	return (s >= 0 && s <= 1 && t >= 0 && t <= 1);
}

/* https://stackoverflow.com/questions/15514906/how-to-check-intersection-between-a-line-and-a-rectangle#19342455
static Point get_line_intersection(Line2D.Double pLine1, Line2D.Double pLine2)
{
    Point
        result = null;

    double
        s1_x = pLine1.x2 - pLine1.x1,
        s1_y = pLine1.y2 - pLine1.y1,

        s2_x = pLine2.x2 - pLine2.x1,
        s2_y = pLine2.y2 - pLine2.y1,

        s = (-s1_y * (pLine1.x1 - pLine2.x1) + s1_x * (pLine1.y1 - pLine2.y1)) / (-s2_x * s1_y + s1_x * s2_y),
        t = ( s2_x * (pLine1.y1 - pLine2.y1) - s2_y * (pLine1.x1 - pLine2.x1)) / (-s2_x * s1_y + s1_x * s2_y);

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
    {
        // Collision detected
        result = new Point(
            (int) (pLine1.x1 + (t * s1_x)),
            (int) (pLine1.y1 + (t * s1_y)));
    }   // end if

    return result;
}*/

void player_t::deployZombie(){
	canShoot = weapon.shootDelay;
	float angle = atan2((float) mousePosition[1] - y, (float) mousePosition[0] - x);

	zombie_t *zed = new zombie_t(gamestate, x, y);
	zed->vx = cos(angle) * 600;
	zed->vy = sin(angle) * 600;
	gamestate->zombies->push_back(zed);

	sf::Packet spawnZombiePacket;
	spawnZombiePacket << PACKET_ADD_ZOMBIE << (int) 1 << zed->id << zed->x << zed->y;
	std::cout << angle << "\n";
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
