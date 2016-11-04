
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include "zombie.hpp"
#include "../world.hpp"

unsigned short zombie_t::ZOMBIE_ID = 0;

zombie_t::zombie_t(gamestate_t *gamestate, float x, float y){
	this->id = zombie_t::ZOMBIE_ID++;
	this->x = x;
	this->y = y;
	this->gamestate = gamestate;

	direction = 1;
}

void zombie_t::update(float delta){
	if(keyState[sf::Keyboard::A])
		vx = -moveSpeed;
	if(keyState[sf::Keyboard::D])
		vx = moveSpeed;

	if(!keyState[sf::Keyboard::A] && !keyState[sf::Keyboard::D]) vx *= 0.75;

	vy += gravity * delta;

	bool onGround = !gamestate->world->placeFree(x, y + vy*delta);

	if(!onGround)
		y += vy * delta;
	else
		vy = 0;
	if(gamestate->world->placeFree(x + vx*delta, y))
		x += vx * delta;
	else
		vx = 0;

	if(onGround && keyState[sf::Keyboard::Space])
	// happy mode enabled
	//if(onGround)
		vy = -450;


	// AI - wandering
	if(gamestate->world->placeFree(x+vx*delta*4, y+1)){
		direction *= -1; // change heading when reaching an edge
		// if(onGround) vy = -450; // just for fun
	}

	keyState[sf::Keyboard::D] = false;
	keyState[sf::Keyboard::A] = false;
	if(direction == 1) keyState[sf::Keyboard::D] = true;
	if(direction == -1) keyState[sf::Keyboard::A] = true;
}
