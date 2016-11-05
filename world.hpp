#ifndef _SAUCECODE_SHOTGUN_WORLD_HPP
#define _SAUCECODE_SHOTGUN_WORLD_HPP

#include <SFML/Graphics.hpp>

#include <vector>

typedef struct {
	float x;
	float y;
	float width;
	float height;
	sf::RectangleShape shape;
} solid_t;

class world_t {

	public:

	std::vector<solid_t*> elements;
	bool isLoaded = false;

	world_t();

	void addElement(solid_t *element);
	void drawElements(sf::RenderWindow *window);

	bool placeFree(float x, float y);
	bool placeFree(float x, float y, solid_t **collider);

};


#endif
