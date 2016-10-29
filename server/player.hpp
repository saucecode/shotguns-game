#ifndef _SAUCECODE_SHOTGUN_SERVER_PLAYER_HPP
#define _SAUCECODE_SHOTGUN_SERVER_PLAYER_HPP

#include <SFML/Network.hpp>

class world_t;

class player_t {
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
	std::string username;

	bool keyState[256];
	unsigned char keyTimers[256];

	sf::IpAddress addr;
	unsigned short port;


	player_t(unsigned short id, float x, float y, std::string username,
		world_t *world);
	void update(float delta);
	void setAddress(sf::UdpSocket *socket, sf::IpAddress addr, unsigned short port);
	void send(sf::Packet packet);
};

#endif
