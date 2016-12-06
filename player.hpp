#ifndef _SAUCECODE_SHOTGUN_PLAYER_HPP
#define _SAUCECODE_SHOTGUN_PLAYER_HPP

#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>

#include <cstdint>

class world_t;
class Game;

class player_t {
	public:

	const float moveSpeed = 200;
	const float gravity = 981;

	bool resourcesLoaded = false;
	float x, y, vx, vy;
	int8_t direction;
	int16_t health;
	int16_t id;
	std::string username;
	bool keyState[256];
	bool mouseState[2];
	int16_t mousePosition[2];
	uint8_t keyTimers[256];

	bool networkKeyState[12];

	sf::IpAddress addr;
	uint16_t port;

	sf::CircleShape shape;
	sf::Sprite sprite;
	sf::Sprite crosshair;

	Game *game;

	player_t(Game *game, int16_t id, float x, float y, std::string username);
	~player_t();

	void update(double delta);
	void draw();

};

#endif
