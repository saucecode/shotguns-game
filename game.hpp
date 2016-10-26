#ifndef _SAUCECODE_SHOTGUN_MAINGAME_HPP
#define _SAUCECODE_SHOTGUN_MAINGAME_HPP

#include <SFML/Graphics.hpp>
#include <thread>
#include <vector>

class player_t;
class Network;

class Game {
	
	private:
	std::vector<player_t*> agents;

	public:
	Game();
	
	void update();
	void render();

	sf::RenderWindow *window;
	player_t *player;
	Network *network;
	
	bool focused = false;
	std::thread *networkThread;

};

#endif