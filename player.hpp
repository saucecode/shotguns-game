#ifndef _SAUCECODE_SHOTGUN_PLAYER_HPP
#define _SAUCECODE_SHOTGUN_PLAYER_HPP

#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>

class player_t {
	public:

	sf::Texture *characterTexture = nullptr;

	const float moveSpeed = 4;
	bool resourcesLoaded = false;
	float x, y, vx, vy;
	char direction;
	short health;
	unsigned short id;
	std::string username;
	bool keyState[256];
	unsigned char keyTimers[256];

	bool networkKeyState[12];

	sf::IpAddress addr;
	unsigned short port;

	sf::CircleShape shape;
	sf::Sprite sprite;

	player_t(unsigned short id, float x, float y, std::string username);
	~player_t();

	void update(double delta);
	void loadResources(sf::Texture *spriteSheet);

};

#endif
