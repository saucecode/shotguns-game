#ifndef _SAUCECODE_SHOTGUN_NETWORK_HPP
#define _SAUCECODE_SHOTGUN_NETWORK_HPP

#include <SFML/Network.hpp>

#include <thread>
#include <cstdint>

class player_t;
class world_t;
class zombie_t;
class Game;

class Network {
	private:
	sf::UdpSocket socket;
	uint16_t port;
	sf::IpAddress host;
	Game *game;

	public:
	bool running = true;
	float latency = 0; // measured in seconds

	// measured in bytes - LAST_*X is reset per second
	uint64_t TX = 0, RX = 0;
	uint64_t DISP_TX = 0, DISP_RX = 0;
	uint64_t LAST_TX=0, LAST_RX = 0;

	Network(Game *game, sf::IpAddress addr, uint16_t port);

	bool connect(std::string username);
	void disconnect();

	void run();
	player_t* getPlayerByID(int16_t id);
	zombie_t* getZombieByID(uint16_t id);
	void stop();
	void send(sf::Packet packet);
};

#endif
