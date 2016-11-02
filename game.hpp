#ifndef _SAUCECODE_SHOTGUN_MAINGAME_HPP
#define _SAUCECODE_SHOTGUN_MAINGAME_HPP

#include <SFML/Graphics.hpp>

#include <thread>
#include <vector>

class player_t;
class zombie_t;
class Network;
class world_t;

class Game {

	private:
	std::vector<player_t*> agents;
	std::vector<zombie_t*> zombies;

	public:
	Game();

	void update(float delta);
	void render(float delta);
	bool startNetworking();

	sf::RenderWindow *window;
	sf::View *playerView;
	sf::Font primaryFont;
	sf::Clock byteCounterClock;
	player_t *player;
	Network *network;

	sf::Text latencyDisplayText;

	bool focused = false;
	std::thread *networkThread;
	world_t *world;

};

#endif
