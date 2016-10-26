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
	
	//player_t player(-1, 0,0, "snowflake");
	//Network network("127.0.0.1", 43234, &agents, &player);
	
	/*try{
		if(network.connect("snowflake")){
			std::cout << "Connected!\n";
		}else{
			std::cout << "Failed to connect. Exiting...\n";
			return 1;
		}
	}catch(std::string ex){
		std::cout << ex << std::endl;
	}*/
	
	//sf::RenderWindow window(sf::VideoMode(320, 320), "SFML works!");
	//bool focused = false;

	//std::thread networkThread(&Network::run, &network);

	while (game.window->isOpen()){
		game.update();
	}
	
	game.network->disconnect();
	game.network->stop();
	game.networkThread->join();
	
	return 0;
}