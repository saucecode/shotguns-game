
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include <iostream>

#include "zombie.hpp"
#include "world.hpp"
#include "game.hpp"

zombie_t::zombie_t(Game *game, unsigned short id, float x, float y, world_t *world){
	this->game = game;
	this->id = id;
	this->x = x;
	this->y = y;
	this->world = world;

	direction = 1;
}

zombie_t::~zombie_t(){
	if(characterTexture != nullptr)
		delete characterTexture;
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

	if(onGround && keyState[sf::Keyboard::Space]){
		vy = -10;
	}
}

void zombie_t::loadResources(sf::Texture *spriteSheet){
	if(resourcesLoaded) return;

	sprite.setTexture(*spriteSheet);
	sprite.setTextureRect(sf::IntRect(17, 8*17, 16, 16));
	sprite.setOrigin(8,16);
	sprite.setScale(sf::Vector2f(2,2));
	resourcesLoaded = true;
}
