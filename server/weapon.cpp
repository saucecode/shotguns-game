#include "weapon.hpp"

#include <SFML/Graphics.hpp>

#include <cmath>
#include <cstdint>

projectile_t::projectile_t(int16_t ownerid, sf::Vector2f start, sf::Vector2f end, float range){
	this->ownerid = ownerid;
	this->start = start;
	this->end = end;
	this->range = range;

	constructVertices();
}

projectile_t::projectile_t(int16_t ownerid, sf::Vector2f start, sf::Vector2f end){
	this->ownerid = ownerid;
	this->start = start;
	this->end = end;
	this->range = hypot(start.x-end.x, start.y-end.y);

	constructVertices();
}

projectile_t::projectile_t(int16_t ownerid, float x, float y, float angle, float range){
	this->ownerid = ownerid;
	this->start.x = x;
	this->start.y = y;
	this->end.x = cos(angle) * range;
	this->end.y = sin(angle) * range;

	constructVertices();
}

projectile_t::projectile_t(int16_t ownerid, sf::Vector2f start, float angle, float range){
	this->ownerid = ownerid;
	this->start = start;
	this->end.x = cos(angle) * range;
	this->end.y = sin(angle) * range;

	constructVertices();
}

void projectile_t::constructVertices(){
	line[0] = start;
	line[1] = end;
}
