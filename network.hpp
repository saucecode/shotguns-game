#ifndef _SAUCECODE_SHOTGUN_NETWORK_HPP
#define _SAUCECODE_SHOTGUN_NETWORK_HPP

#include <SFML/Network.hpp>
#include <thread>
#include <mutex>

#include "player.hpp"

class Network {
	private:
	sf::UdpSocket socket;
	unsigned short port;
	sf::IpAddress host;
	std::vector<player_t*> *agents;
	player_t *player;
	
	public:
	bool running = true;
		
	Network(sf::IpAddress addr, unsigned short port, std::vector<player_t*> *agents, player_t *player);
	
	bool connect(std::string username);
	void disconnect();
	
	void run();
	player_t* getPlayerByID(unsigned short id);
	void stop();
	void send(sf::Packet packet);
};

#endif