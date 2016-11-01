
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include "zombie.hpp"
#include "../world.hpp"

zombie_t::zombie_t(unsigned short id, float x, float y, world_t *world){
	this->id = id;
	this->x = x;
	this->y = y;
	this->world = world;

	direction = 1;
}

void zombie_t::update(float delta){
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

	if(onGround && keyState[sf::Keyboard::Space])
	// happy mode enabled
	//if(onGround)
		vy = -10;


	// AI - wandering
	if(world->placeFree(x+vx*4, y+1)) direction *= -1; // change heading when reaching an edge

	keyState[sf::Keyboard::D] = false;
	keyState[sf::Keyboard::A] = false;
	if(direction == 1) keyState[sf::Keyboard::D] = true;
	if(direction == -1) keyState[sf::Keyboard::A] = true;
}
