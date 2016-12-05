#ifndef _SAUCECODE_SHOTGUN_MAINGAME_HPP
#define _SAUCECODE_SHOTGUN_MAINGAME_HPP

#include <SFML/Graphics.hpp>

#include <thread>
#include <vector>
#include <mutex>
#include <cstdint>

class player_t;
class zombie_t;
class Network;
class world_t;
class projectile_t;
class ResourceManager;

class Game {

	private:
	const int32_t WINDOW_WIDTH = 640;
	const int32_t WINDOW_HEIGHT = 480;

	public:

	std::vector<player_t*> agents;
	std::mutex agentsMutex;
	std::vector<zombie_t*> zombies;
	std::mutex zombiesMutex;
	std::vector<projectile_t*> projectiles;
	std::mutex projectilesMutex;

	Game();

	void update(float delta);
	void render(float delta);
	bool startNetworking();

	sf::RenderWindow *window;
	sf::View *playerView;
	sf::Font primaryFont;

	sf::Texture spriteSheet;

	sf::Clock byteCounterClock;
	player_t *player;
	Network *network;
	ResourceManager *resourceManager;

	sf::Text latencyDisplayText;

	bool focused = false;
	std::thread *networkThread;
	world_t *world;

};

#endif
