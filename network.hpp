#ifndef _SAUCECODE_SHOTGUN_NETWORK_HPP
#define _SAUCECODE_SHOTGUN_NETWORK_HPP

#include <SFML/Network.hpp>

#include <thread>

class player_t;
class world_t;
class zombie_t;
class Game;

class Network {
	private:
	sf::UdpSocket socket;
	unsigned short port;
	sf::IpAddress host;
	std::vector<player_t*> *agents;
	std::vector<zombie_t*> *zombies;
	player_t *player;
	world_t *world;
	Game *game;

	public:
	bool running = true;
	float latency = 0; // measured in seconds

	// measured in bytes - LAST_*X is reset per second
	unsigned long long TX = 0, RX = 0;
	unsigned long long DISP_TX = 0, DISP_RX = 0;
	unsigned long long LAST_TX=0, LAST_RX = 0;

	Network(Game *game, sf::IpAddress addr, unsigned short port, std::vector<player_t*> *agents, std::vector<zombie_t*> *zombies, player_t *player, world_t *world);

	bool connect(std::string username);
	void disconnect();

	void run();
	player_t* getPlayerByID(unsigned short id);
	zombie_t* getZombieByID(unsigned short id);
	void stop();
	void send(sf::Packet packet);
};

#endif
