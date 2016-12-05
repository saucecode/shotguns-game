#ifndef _SAUCECODE_SHOTGUN_ZOMBIE_HPP
#define _SAUCECODE_SHOTGUN_ZOMBIE_HPP

#include <cstdint>

class world_t;
class Game;

class zombie_t {
	private:
	sf::UdpSocket *socket;

	public:
	const float moveSpeed = 200;
	const float gravity = 32;

	float x, y, vx=0, vy=0;
	float width = 32, height = 32;
	int8_t direction;
	int16_t health=1000;
	uint16_t id;
	bool resourcesLoaded = false;

	bool keyState[256];
	uint8_t keyTimers[256];

	sf::Sprite sprite;

	Game *game;

	zombie_t(Game *game, uint16_t id, float x, float y);
	~zombie_t();
	void update(float delta);
	void draw();

};

#endif
