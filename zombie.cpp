
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include <iostream>

#include "zombie.hpp"
#include "world.hpp"
#include "game.hpp"
#include "resource_manager.hpp"


zombie_t::zombie_t(Game *game, unsigned short id, float x, float y){
	this->game = game;
	this->id = id;
	this->x = x;
	this->y = y;

	direction = 1;

	sprite.setTexture(*(game->resourceManager->textures.at("zombie")));
	sprite.setOrigin(sprite.getTexture()->getSize().x/2, sprite.getTexture()->getSize().y/2);
	sprite.setScale(sf::Vector2f(2,2));
}

zombie_t::~zombie_t(){
	// pass
}

void zombie_t::update(float delta){
	if(keyState[sf::Keyboard::A])
		vx = -moveSpeed * delta;
	if(keyState[sf::Keyboard::D])
		vx = moveSpeed * delta;

	if(!keyState[sf::Keyboard::A] && !keyState[sf::Keyboard::D]) vx *= 0.75;

	vy += gravity * delta;

	bool onGround = !game->world->placeFree(x, y + vy);

	if(!onGround)
		y += vy;
	else
		vy = 0;
	if(game->world->placeFree(x + vx, y))
		x += vx;
	else
		vx = 0;

	if(onGround && keyState[sf::Keyboard::Space]){
		vy = -10;
	}
}

void zombie_t::draw(){
	// draw zombie sprite
	sprite.setPosition(x,y);
	game->window->draw(sprite);
}
