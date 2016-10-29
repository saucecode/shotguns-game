#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <iostream>

#include "network.hpp"
#include "packetid.hpp"

#include "player.hpp"
#include "game.hpp"

std::vector<player_t*> agents;

int main(){

	Game game;

	game.startNetworking();

	sf::Clock deltaTimer;

	while (game.window->isOpen()){
		float delta = deltaTimer.restart().asSeconds();
		// std::cout << delta << "\n";
		game.update(delta);
		game.render(delta);
	}

	game.network->disconnect();
	game.network->stop();
	game.networkThread->join();

	return 0;
}
