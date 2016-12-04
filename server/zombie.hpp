#ifndef _SAUCECODE_SHOTGUN_SERVER_ZOMBIE_HPP
#define _SAUCECODE_SHOTGUN_SERVER_ZOMBIE_HPP

#include "gamestate.hpp"

class world_t;

class zombie_t {
	private:
	sf::UdpSocket *socket;

	static unsigned short ZOMBIE_ID;

	public:

	static const char AI_WANDERING, AI_CHASE;

	const float moveSpeed = 100;
	const float gravity = 981;

	float x, y, vx=0, vy=0;
	float width = 32, height = 32;
	char direction;
	short health = 1000;
	unsigned short id;
	char state;
	float deltaAccumulator = 0;

	gamestate_t *gamestate;

	bool keyState[256];
	unsigned char keyTimers[256];

	zombie_t(gamestate_t *gamestate, float x, float y);
	void update(float delta);
};

#endif
