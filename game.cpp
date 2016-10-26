
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <thread>

#include "game.hpp"
#include "player.hpp"
#include "network.hpp"
#include "packetid.hpp"

Game::Game(){
	player = new player_t(-1, 0,0, "snowflake");
	network = new Network("127.0.0.1", 43234, &agents, player);
	
	try{
		if(network->connect("snowflake")){
			std::cout << "Connected!\n";
		}else{
			std::cout << "Failed to connect. Exiting...\n";
		}
	}catch(std::string ex){
		std::cout << ex << std::endl;
	}
	
	window = new sf::RenderWindow(sf::VideoMode(320, 320), "SFML works!");
	
	
	networkThread = new std::thread(&Network::run, network);
}

void Game::update(){
	sf::Event event;
	while (window->pollEvent(event)){
		if (event.type == sf::Event::Closed)
			window->close();
		if(event.type == sf::Event::GainedFocus){
			focused = true;
		}
		if(event.type == sf::Event::LostFocus){
			focused = false;
		}
	}
	
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::D) && focused){
		sf::Packet packet;
		packet << PACKET_MOVE_PLAYER << sf::Int8('D') << sf::Uint8(player->moveSpeed);
		network->send(packet);
	}
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::A) && focused){
		sf::Packet packet;
		packet << PACKET_MOVE_PLAYER << sf::Int8('A') << sf::Uint8(player->moveSpeed);
		network->send(packet);
	}
	window->clear();
	
	player->shape.setPosition(player->x, player->y);
	window->draw(player->shape);
	
	for(player_t *agent : agents){
		agent->shape.setPosition(agent->x, agent->y);
		window->draw(agent->shape);
	}
	
	window->display();
	std::this_thread::sleep_for(std::chrono::milliseconds(20));
}