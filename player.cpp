#include "player.hpp"
#include <string>
#include <iostream>
#include <SFML/Graphics.hpp>

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

void player_t::loadResources(){
	if(resourcesLoaded) return;

	/*
		NOTE:
		This code used to be in the constructor.
		When players joined, player_t constructor is called
		from the network thread, and it attempts to load the
		textures.

		This crashes the client! Load resources in the render
		thread!
	*/

	if(characterTexture == nullptr){
		characterTexture = new sf::Texture();
		if(!characterTexture->loadFromFile("res/character.png")){
			std::cout << "FUUUUCK\n";
		}
	}

	sprite.setTexture(*characterTexture);
	sprite.setTextureRect(sf::IntRect(0, 0, 16, 16));
	sprite.setOrigin(8,16);
	sprite.setScale(sf::Vector2f(2,2));
	resourcesLoaded = true;
}

void player_t::update(double delta){
	x += vx;
	y += vy;

}
