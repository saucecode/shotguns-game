#ifndef _SAUCECODE_SHOTGUN_SERVER_PLAYER_HPP
#define _SAUCECODE_SHOTGUN_SERVER_PLAYER_HPP

#include <SFML/Network.hpp>

#include <cstdint>

#include "weapon.hpp"
#include "gamestate.hpp"

class world_t;
class zombie_t;

class player_t {
	private:
	sf::UdpSocket *socket;

	static int16_t PLAYER_ID;

	public:
	const float moveSpeed = 200;
	const float gravity = 981;
	float weaponHeight = 10;

	float x, y, vx=0, vy=0;
	float width = 32, height = 32;
	float lastSentX=0, lastSentY=0;
	int8_t direction;
	int16_t health;
	int16_t id;
	std::string username;
	gamestate_t *gamestate;

	weapon_t weapon;
	float canShoot = 0; // seconds

	bool hasDownloadedWorld = false;

	int32_t pingTicker = 0;
	float latency = 0; // seconds
	sf::Clock pingClock;

	bool keyState[256];
	bool mouseState[2];
	int16_t mousePosition[2];
	uint8_t keyTimers[256];

	sf::IpAddress addr;
	uint16_t port;


	player_t(gamestate_t *gamestate, float x, float y, std::string username);
	void update(float delta);
	void deployZombie();
	void shoot();
	projectile_t hitscan(int16_t ownerid, world_t *world, float x, float y, float angle, const float range);
	void setAddress(sf::UdpSocket *socket, sf::IpAddress addr, uint16_t port);
	void send(sf::Packet packet);
	void strike(sf::Vector2f impulse);


	static bool lineIntersection(sf::Vector2f origin1, sf::Vector2f dest1,
		sf::Vector2f origin2, sf::Vector2f dest2);
};

#endif
