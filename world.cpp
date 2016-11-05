
#include <SFML/Graphics.hpp>

#include "world.hpp"

world_t::world_t(){

}

void world_t::addElement(solid_t *element){
	elements.push_back(element);

	// element->shape = sf::RectangleShape(element->x, element->y, element->width, element->height);
	element->shape.setPosition(sf::Vector2f(element->x, element->y));
	element->shape.setSize(sf::Vector2f(element->width, element->height));
}

bool world_t::placeFree(float x, float y){
	for(solid_t *solid : elements){
		if(x > solid->x && x < solid->x+solid->width
		&& y > solid->y && y < solid->y+solid->height)
			return false;
	}
	return true;
}

bool world_t::placeFree(float x, float y, solid_t *collision){
	for(solid_t *solid : elements){
		if(x > solid->x && x < solid->x+solid->width
		&& y > solid->y && y < solid->y+solid->height){
			collision = solid;
			return false;
		}
	}
	return true;
}

void world_t::drawElements(sf::RenderWindow *window){
	for(solid_t *element : elements){
		window->draw(element->shape);
	}
}
