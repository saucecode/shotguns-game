
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include <iostream>

#include "zombie.hpp"
#include "../world.hpp"

unsigned short zombie_t::ZOMBIE_ID = 0;
const char zombie_t::AI_WANDERING = 'W', zombie_t::AI_CHASE = 'C';

zombie_t::zombie_t(gamestate_t *gamestate, float x, float y){
	this->id = zombie_t::ZOMBIE_ID++;
	this->x = x;
	this->y = y;
	this->gamestate = gamestate;

	this->state = zombie_t::AI_WANDERING;

	direction = 1;
}

void zombie_t::update(float delta){
	/*
		____  ____  __  ______  ________ ________  ___
	   /_  / / __ \/  |/  / _ )/  _/ __// ___/ _ \/ _ \
	    / /_/ /_/ / /|_/ / _  |/ // _/_/ /__/ ___/ ___/
	   /___/\____/_/  /_/____/___/___(_)___/_/  /_/
	*/

	solid_t *ground;
	bool onGround = !gamestate->world->placeFree(x, y + height*0.5 + vy*delta + 0.01, &ground);
	if(onGround) y = ground->y - height*0.5;

	// translate keyboard inputs

	if(keyState[sf::Keyboard::A])
		vx += -moveSpeed * delta;
	if(keyState[sf::Keyboard::D])
		vx += moveSpeed * delta;
	if(onGround && keyState[sf::Keyboard::Space])
		vy = -450;
	if(!onGround) keyState[sf::Keyboard::Space] = false;


	// physics environment

	vy += gravity * delta;
	if(onGround && !keyState[sf::Keyboard::A] && !keyState[sf::Keyboard::D]) vx *= 0.95;

	if(onGround && vx > moveSpeed) vx = moveSpeed;
	if(onGround && vx < -moveSpeed) vx = -moveSpeed;

	if(!onGround){
		y += vy * delta;
	}else{
		vy = 0;
	}

	if(gamestate->world->placeFree(x + vx*delta, y + height*0.5))
		x += vx * delta;
	else
		vx = 0;

	// do not clip through platforms when moving up
	if(vy < 0){
		if(!gamestate->world->placeFree(x + vx*delta, y + vy*delta)){
			vy = 0;
		}
	}


	// AI - wandering
	if(state == zombie_t::AI_WANDERING){
		// change heading when reaching an edge
		if(gamestate->world->placeFree(x+vx*delta*4, y+1 + height*0.5) && onGround){
			direction *= -1;
			vx *= -1;
		}
	}else if(state == zombie_t::AI_CHASE){
		// when chasing someone, jump when you reach an edge
		if(gamestate->world->placeFree(x+vx*delta*4, y+1 + height*0.5) && onGround){
			keyState[sf::Keyboard::Space] = true;
		}
	}

	keyState[sf::Keyboard::D] = false;
	keyState[sf::Keyboard::A] = false;
	if(onGround && direction == 1) keyState[sf::Keyboard::D] = true;
	if(onGround && direction == -1) keyState[sf::Keyboard::A] = true;
}
