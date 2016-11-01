#ifndef _SAUCECODE_SHOTGUN_NETWORK_HPP
#define _SAUCECODE_SHOTGUN_NETWORK_HPP

#include <SFML/Network.hpp>

#include <thread>

class player_t;
class world_t;
class zombie_t;

class Network {
	private:
	sf::UdpSocket socket;
	unsigned short port;
	sf::IpAddress host;
	std::vector<player_t*> *agents;
	std::vector<zombie_t*> *zombies;
	player_t *player;
	world_t *world;

	public:
	bool running = true;

	Network(sf::IpAddress addr, unsigned short port, std::vector<player_t*> *agents, std::vector<zombie_t*> *zombies, player_t *player, world_t *world);

	bool connect(std::string username);
	void disconnect();

	void run();
	player_t* getPlayerByID(unsigned short id);
	zombie_t* getZombieByID(unsigned short id);
	void stop();
	void send(sf::Packet packet);
};

#endif
