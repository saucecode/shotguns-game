#include "weapon.hpp"

#include <cmath>

projectile_t::projectile_t(sf::Vector2f start, sf::Vector2f end, float range){
	this->start = start;
	this->end = end;
	this->range = range;
}

projectile_t::projectile_t(sf::Vector2f start, sf::Vector2f end){
	this->start = start;
	this->end = end;
	this->range = hypot(start.x-end.x, start.y-end.y);
}

projectile_t::projectile_t(float x, float y, float angle, float range){
	this->start.x = x;
	this->start.y = y;
	this->end.x = cos(angle) * range;
	this->end.y = sin(angle) * range;
}

projectile_t::projectile_t(sf::Vector2f start, float angle, float range){
	this->start = start;
	this->end.x = cos(angle) * range;
	this->end.y = sin(angle) * range;
}
