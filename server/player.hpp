#ifndef _SAUCECODE_SHOTGUN_SERVER_PLAYER_HPP
#define _SAUCECODE_SHOTGUN_SERVER_PLAYER_HPP

#include <SFML/Network.hpp>

class player_t {
	private:
	sf::UdpSocket *socket;
	
	public:
	float x, y;
	char direction;
	short health;
	unsigned short id;
	std::string username;
	
	sf::IpAddress addr;
	unsigned short port;
	
	
	player_t(unsigned short id, float x, float y, std::string username);
	void setAddress(sf::UdpSocket *socket, sf::IpAddress addr, unsigned short port);
	void sendPacket(sf::Packet packet);
};

#endif