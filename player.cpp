
#include <SFML/Graphics.hpp>

#include <string>
#include <iostream>

#include "player.hpp"

player_t::player_t(unsigned short id, float x, float y, std::string username){
	this->id = id;
	this->x = x;
	this->y = y;
	this->vx = 0;
	this->vy = 0;
	this->username = username;

	shape = sf::CircleShape(8.0f);
	shape.setFillColor(sf::Color::White);
	shape.setOrigin(x + shape.getRadius(),y + shape.getRadius()*2);
}

player_t::~player_t(){
	// pass
}

void player_t::loadResources(sf::Texture *spriteSheet){
	if(resourcesLoaded) return;

	sprite.setTexture(*spriteSheet);
	sprite.setTextureRect(sf::IntRect(0, 5*17, 16, 16));
	sprite.setOrigin(8,16);
	sprite.setScale(sf::Vector2f(2,2));
	resourcesLoaded = true;
}

void player_t::update(double delta){
	x += vx;
	y += vy;

}
