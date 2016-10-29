#ifndef _SAUCECODE_SHOTGUN_MAINGAME_HPP
#define _SAUCECODE_SHOTGUN_MAINGAME_HPP

#include <SFML/Graphics.hpp>
#include <thread>
#include <vector>

class player_t;
class Network;
class world_t;

class Game {

	private:
	std::vector<player_t*> agents;

	public:
	Game();

	void update(float delta);
	void render(float delta);
	bool startNetworking();

	sf::RenderWindow *window;
	sf::View *playerView;
	player_t *player;
	Network *network;

	bool focused = false;
	std::thread *networkThread;
	world_t *world;

};

#endif
