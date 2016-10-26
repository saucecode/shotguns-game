#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <iostream>

#include "network.hpp"
#include "packetid.hpp"

#include "player.hpp"

std::vector<player_t*> agents;

int main(){

	player_t player(-1, 0,0, "snowflake");
	Network network("127.0.0.1", 43234, &agents, &player);
	
	try{
		if(network.connect("snowflake")){
			std::cout << "Connected!\n";
		}else{
			std::cout << "Failed to connect. Exiting...\n";
			return 1;
		}
	}catch(std::string ex){
		std::cout << ex << std::endl;
	}
	
	sf::RenderWindow window(sf::VideoMode(320, 320), "SFML works!");
	bool focused = false;

	std::thread networkThread(&Network::run, &network);

	while (window.isOpen()){
		sf::Event event;
		while (window.pollEvent(event)){
			if (event.type == sf::Event::Closed)
				window.close();
			if(event.type == sf::Event::GainedFocus){
				focused = true;
			}
			if(event.type == sf::Event::LostFocus){
				focused = false;
			}
		}
		
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::D) && focused){
			sf::Packet packet;
			packet << PACKET_MOVE_PLAYER << sf::Int8('D') << sf::Uint8(player.moveSpeed);
			//player.x+=player.moveSpeed;
			network.send(packet);
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::A) && focused){
			sf::Packet packet;
			packet << PACKET_MOVE_PLAYER << sf::Int8('A') << sf::Uint8(player.moveSpeed);
			//player.x-=player.moveSpeed;
			network.send(packet);
		}

		window.clear();
		
		player.shape.setPosition(player.x, player.y);
		window.draw(player.shape);
		
		for(player_t *player : agents){
			player->shape.setPosition(player->x, player->y);
			window.draw(player->shape);
		}
		
		window.display();
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	
	network.disconnect();
	network.stop();
	networkThread.join();
	
	return 0;
}