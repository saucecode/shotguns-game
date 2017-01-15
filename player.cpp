
#include <SFML/Graphics.hpp>

#include <string>
#include <iostream>

#include "player.hpp"
#include "world.hpp"
#include "game.hpp"
#include "resource_manager.hpp"

player_t::player_t(Game *game, int16_t id, float x, float y, std::string username){
	this->game = game;
	this->id = id;
	this->x = x;
	this->y = y;
	this->vx = 0;
	this->vy = 0;
	this->username = username;

	shape = sf::CircleShape(8.0f);
	shape.setFillColor(sf::Color::White);
	shape.setOrigin(x + shape.getRadius(),y + shape.getRadius()*2);

	sprite.setTexture(*(game->resourceManager->textures.at("character")));
	sprite.setOrigin(sprite.getTexture()->getSize().x*0.5, sprite.getTexture()->getSize().y*0.5);
	sprite.setScale(sf::Vector2f(2,2));

	if(id == -1){
		crosshair.setTexture(*(game->resourceManager->textures.at("crosshair")));
		// set origin to center of texture
		crosshair.setOrigin(crosshair.getTexture()->getSize().x/2, crosshair.getTexture()->getSize().y/2);
	}

	for (int i=0; i<256; i++) {
		keyState[i] = false;
	}
}

player_t::~player_t(){
	// pass
}

void player_t::update(double delta){
	if(keyState[sf::Keyboard::A])
		vx = -moveSpeed;
	if(keyState[sf::Keyboard::D])
		vx = moveSpeed;

	if(!keyState[sf::Keyboard::A] && !keyState[sf::Keyboard::D]) vx *= 0.75;

	bool onGround = !game->world->placeFree(x, y + vy*delta + 0.05);

	if(!onGround)
		y += vy*delta;
	else
		vy = 0;
	if(game->world->placeFree(x + vx*delta, y))
		x += vx*delta;
	else
		vx = 0;

	if(onGround && keyState[sf::Keyboard::Space]){
		vy = -450; // jump_velocity = sqrt(2 * gravity * maximum_height)
	}
}

void player_t::draw(){
	// draw player character
	this->sprite.setPosition(x,y);
	game->window->draw(sprite);
	if(id == -1){
		crosshair.setPosition(mousePosition[0], mousePosition[1]);
		game->window->draw(crosshair);
	}
}
