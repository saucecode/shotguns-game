#ifndef _SAUCECODE_SHOTGUN_SERVER_ZOMBIE_HPP
#define _SAUCECODE_SHOTGUN_SERVER_ZOMBIE_HPP

#include "gamestate.hpp"

class world_t;

class zombie_t {
	private:
	sf::UdpSocket *socket;

	static uint16_t ZOMBIE_ID;

	public:

	static const int8_t AI_WANDERING, AI_CHASE;

	const float moveSpeed = 100;
	const float gravity = 981;

	float x, y, vx=0, vy=0;
	float width = 32, height = 32;
	int8_t direction;
	int16_t health = 1000;
	uint16_t id;
	int8_t state;
	float deltaAccumulator = 0;

	gamestate_t *gamestate;

	bool keyState[256];
	uint8_t keyTimers[256];

	zombie_t(gamestate_t *gamestate, float x, float y);
	void update(float delta);
};

#endif
