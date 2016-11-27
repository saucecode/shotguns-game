#ifndef _SAUCECODE_SHOTGUN_PLAYER_HPP
#define _SAUCECODE_SHOTGUN_PLAYER_HPP

#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>

class world_t;
class Game;

class player_t {
	public:

	const float moveSpeed = 200;
	const float gravity = 981;

	sf::Texture *characterTexture = nullptr;

	bool resourcesLoaded = false;
	float x, y, vx, vy;
	char direction;
	short health;
	unsigned short id;
	std::string username;
	bool keyState[256];
	bool mouseState[2];
	short mousePosition[2];
	unsigned char keyTimers[256];

	bool networkKeyState[12];

	sf::IpAddress addr;
	unsigned short port;

	sf::CircleShape shape;
	sf::Sprite sprite;

	world_t *world;
	Game *game;

	player_t(Game *game, unsigned short id, float x, float y, std::string username, world_t *world);
	~player_t();

	void update(double delta);
	void draw();
	void loadResources(sf::Texture *spriteSheet);

};

#endif
