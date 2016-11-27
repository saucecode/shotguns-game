#ifndef _SAUCECODE_SHOTGUN_ZOMBIE_HPP
#define _SAUCECODE_SHOTGUN_ZOMBIE_HPP

class world_t;
class Game;

class zombie_t {
	private:
	sf::UdpSocket *socket;
	world_t *world;

	public:
	const float moveSpeed = 200;
	const float gravity = 32;

	float x, y, vx=0, vy=0;
	char direction;
	short health;
	unsigned short id;
	bool resourcesLoaded = false;

	bool keyState[256];
	unsigned char keyTimers[256];

	sf::Sprite sprite;
	sf::Texture *characterTexture = nullptr;

	Game *game;

	zombie_t(Game *game, unsigned short id, float x, float y, world_t *world);
	~zombie_t();
	void update(float delta);
	void loadResources(sf::Texture *spriteSheet);
};

#endif
