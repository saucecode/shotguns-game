
#include <SFML/Graphics.hpp>

#include <string>
#include <iostream>

#include "player.hpp"
#include "world.hpp"
#include "game.hpp"
#include "resource_manager.hpp"

player_t::player_t(Game *game, unsigned short id, float x, float y, std::string username, world_t *world){
	this->game = game;
	this->id = id;
	this->x = x;
	this->y = y;
	this->vx = 0;
	this->vy = 0;
	this->username = username;
	this->world = world;

	shape = sf::CircleShape(8.0f);
	shape.setFillColor(sf::Color::White);
	shape.setOrigin(x + shape.getRadius(),y + shape.getRadius()*2);

	sprite.setTexture(*(game->resourceManager->textures.at("character")));
	sprite.setOrigin(8,16);
	sprite.setScale(sf::Vector2f(2,2));

	crosshair.setTexture(*(game->resourceManager->textures.at("crosshair")));
	// set origin to center of texture
	crosshair.setOrigin(crosshair.getTexture()->getSize().x/2, crosshair.getTexture()->getSize().y/2);
}

player_t::~player_t(){
	// pass
}

// ** DEPRECATED **
void player_t::loadResources(sf::Texture *spriteSheet){
	if(resourcesLoaded) return;

	sprite.setTexture(*spriteSheet);
	sprite.setTextureRect(sf::IntRect(0, 5*17, 16, 16));
	sprite.setOrigin(8,16);
	sprite.setScale(sf::Vector2f(2,2));
	resourcesLoaded = true;
}

void player_t::update(double delta){

	crosshair.setPosition(mousePosition[0], mousePosition[1]);

	if(keyState[sf::Keyboard::A])
		vx = -moveSpeed;
	if(keyState[sf::Keyboard::D])
		vx = moveSpeed;

	if(!keyState[sf::Keyboard::A] && !keyState[sf::Keyboard::D]) vx *= 0.75;

	bool onGround = !world->placeFree(x, y + vy*delta + 0.05);

	if(!onGround)
		y += vy*delta;
	else
		vy = 0;
	if(world->placeFree(x + vx*delta, y))
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
	game->window->draw(crosshair);
}
