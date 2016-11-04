#ifndef _SAUCECODE_SHOTGUN_SERVER_PLAYER_HPP
#define _SAUCECODE_SHOTGUN_SERVER_PLAYER_HPP

#include <SFML/Network.hpp>

#include "weapon.hpp"
#include "gamestate.hpp"

class world_t;
class zombie_t;

class player_t {
	private:
	sf::UdpSocket *socket;

	static unsigned short PLAYER_ID;

	public:
	const float moveSpeed = 200;
	const float gravity = 981;

	float x, y, vx=0, vy=0;
	float lastSentX=0, lastSentY=0;
	char direction;
	short health;
	unsigned short id;
	std::string username;
	gamestate_t *gamestate;

	weapon_t weapon;
	float canShoot = 0; // seconds

	bool hasDownloadedWorld = false;

	int pingTicker = 0;
	float latency = 0; // seconds
	sf::Clock pingClock;

	bool keyState[256];
	bool mouseState[2];
	short mousePosition[2];
	unsigned char keyTimers[256];

	sf::IpAddress addr;
	unsigned short port;


	player_t(gamestate_t *gamestate, float x, float y, std::string username);
	void update(float delta);
	void shoot();
	void setAddress(sf::UdpSocket *socket, sf::IpAddress addr, unsigned short port);
	void send(sf::Packet packet);
};

#endif
