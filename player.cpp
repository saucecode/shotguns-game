#include "player.hpp"
#include <string>
#include <SFML/Graphics.hpp>

player_t::player_t(unsigned short id, float x, float y, std::string username){
	this->id = id;
	this->x = x;
	this->y = y;
	this->username = username;
	shape = sf::CircleShape(8.0f);
	shape.setFillColor(sf::Color::White);
}