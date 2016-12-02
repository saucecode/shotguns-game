#include "weapon.hpp"

#include <SFML/Graphics.hpp>

#include <cmath>

projectile_t::projectile_t(player_t *owner, sf::Vector2f start, sf::Vector2f end, float range){
	this->owner = owner;
	this->start = start;
	this->end = end;
	this->range = range;

	constructVertices();
}

projectile_t::projectile_t(player_t *owner, sf::Vector2f start, sf::Vector2f end){
	this->owner = owner;
	this->start = start;
	this->end = end;
	this->range = hypot(start.x-end.x, start.y-end.y);

	constructVertices();
}

projectile_t::projectile_t(player_t *owner, float x, float y, float angle, float range){
	this->owner = owner;
	this->start.x = x;
	this->start.y = y;
	this->end.x = cos(angle) * range;
	this->end.y = sin(angle) * range;

	constructVertices();
}

projectile_t::projectile_t(player_t *owner, sf::Vector2f start, float angle, float range){
	this->owner = owner;
	this->start = start;
	this->end.x = cos(angle) * range;
	this->end.y = sin(angle) * range;

	constructVertices();
}

void projectile_t::constructVertices(){
	line[0] = start;
	line[1] = end;
}
