#ifndef _SAUCECODE_SHOTGUN_WORLD_HPP
#define _SAUCECODE_SHOTGUN_WORLD_HPP

#include <vector>
#include <SFML/Graphics.hpp>

typedef struct {
	float x;
	float y;
	float width;
	float height;
	sf::RectangleShape shape;
} solid_t;

class world_t {

	private:

	std::vector<solid_t*> elements;

	public:

	world_t();
	~world_t();

	void addElement(solid_t *element);
	void drawElements(sf::RenderWindow *window);

	bool placeFree(float x, float y);

};


#endif
