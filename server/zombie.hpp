#ifndef _SAUCECODE_SHOTGUN_SERVER_ZOMBIE_HPP
#define _SAUCECODE_SHOTGUN_SERVER_ZOMBIE_HPP

class world_t;

class zombie_t {
	private:
	sf::UdpSocket *socket;
	world_t *world;

	public:
	const float moveSpeed = 100;
	const float gravity = 981;

	float x, y, vx=0, vy=0;
	char direction;
	short health;
	unsigned short id;
	float deltaAccumulator = 0;

	bool keyState[256];
	unsigned char keyTimers[256];

	zombie_t(unsigned short id, float x, float y, world_t *world);
	void update(float delta);
};

#endif
