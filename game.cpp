
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <thread>

#include "game.hpp"
#include "player.hpp"
#include "network.hpp"
#include "packetid.hpp"
#include "world.hpp"

Game::Game(){
	world = new world_t();
	player = new player_t(-1, 0,0, "snowflake");
	network = new Network("127.0.0.1", 43234, &agents, player, world);

	window = new sf::RenderWindow(sf::VideoMode(320, 320), "SFML works!");
	playerView = new sf::View(sf::FloatRect(0,0,320,320));
	playerView->setCenter(player->x, player->y);

	window->setView(*playerView);

	world->addElement(new solid_t{-100,100,200,200});
	world->addElement(new solid_t{132,100,64,16});
	world->addElement(new solid_t{132+64,64,64,128});

}

bool Game::startNetworking(){
	try{
		if(network->connect("snowflake")){
			std::cout << "Connected!\n";
			networkThread = new std::thread(&Network::run, network);
			return true;
		}else{
			std::cout << "Failed to connect. Exiting...\n";
			return false;
		}
	}catch(const std::exception &ex){
		std::cout << ex.what() << std::endl;
	}

	return false;
}

void Game::update(float delta){
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

	if(focused){
		for(int i=0; i<256; i++){
			player->keyState[i] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key(i));
		}

		// generate keyboard snapshot blob
		sf::Packet keyStateBlob;
		keyStateBlob << PACKET_KEY_STATE;
		keyStateBlob << player->keyState[sf::Keyboard::W];
		keyStateBlob << player->keyState[sf::Keyboard::A];
		keyStateBlob << player->keyState[sf::Keyboard::S];
		keyStateBlob << player->keyState[sf::Keyboard::D];
		keyStateBlob << player->keyState[sf::Keyboard::Space];

		network->send(keyStateBlob);
	}

}

void Game::render(float delta){
	window->clear();

	playerView->setCenter(player->x, player->y);
	window->setView(*playerView);

	player->shape.setPosition(player->x, player->y);
	window->draw(player->shape);

	for(player_t *agent : agents){
		agent->shape.setPosition(agent->x, agent->y);
		window->draw(agent->shape);
	}

	world->drawElements(window);

	window->display();
	std::this_thread::sleep_for(std::chrono::milliseconds(20));
}
