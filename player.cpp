#include "player.hpp"
#include <string>
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

void player_t::update(double delta){
	x += vx;
	y += vy;

}
