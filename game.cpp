
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include <iostream>
#include <thread>

#include "game.hpp"
#include "player.hpp"
#include "network.hpp"
#include "packetid.hpp"
#include "world.hpp"
#include "zombie.hpp"
#include "resource_manager.hpp"

Game::Game(){
	this->resourceManager = new ResourceManager();
	resourceManager->loadResourcesFolder("res");

	world = new world_t();
	player = new player_t(this, -1, 0,0, "snowflake", world);
	network = new Network(this, "127.0.0.1", 43234, &agents, &zombies, player, world);

	window = new sf::RenderWindow(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "SFML works!");
	playerView = new sf::View(sf::FloatRect(0,0,WINDOW_WIDTH, WINDOW_HEIGHT));
	playerView->setCenter(player->x, player->y);

	window->setView(*playerView);
	//window->setVerticalSyncEnabled(true);
	window->setFramerateLimit(60);

	if(!primaryFont.loadFromFile("res/kenpixel_mini.ttf"))
		std::cout << "Failed to load font\n";

	latencyDisplayText.setFont(primaryFont);
	latencyDisplayText.setString("Latency: ?");
	latencyDisplayText.setColor(sf::Color::White);
	latencyDisplayText.setCharacterSize(16);

	if(!spriteSheet.loadFromFile("res/roguelikeChar_transparent.png")){
		std::cout << "Failed to load spritesheet\n";
	}
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
		player->mouseState[0] = sf::Mouse::isButtonPressed(sf::Mouse::Left);
		player->mouseState[1] = sf::Mouse::isButtonPressed(sf::Mouse::Right);

		sf::Vector2i mousePosition = sf::Mouse::getPosition(*window);
		player->mousePosition[0] = (short) (mousePosition.x + (int) player->x - WINDOW_WIDTH/2);
		player->mousePosition[1] = (short) (mousePosition.y + (int) player->y - WINDOW_HEIGHT/2);

		// generate keyboard snapshot blob
		sf::Packet keyStateBlob;
		keyStateBlob << PACKET_KEY_STATE;

		keyStateBlob << player->keyState[sf::Keyboard::W];
		keyStateBlob << player->keyState[sf::Keyboard::A];
		keyStateBlob << player->keyState[sf::Keyboard::S];
		keyStateBlob << player->keyState[sf::Keyboard::D];
		keyStateBlob << player->keyState[sf::Keyboard::Space];
		keyStateBlob << player->keyState[sf::Keyboard::Q];
		keyStateBlob << player->keyState[sf::Keyboard::E];
		keyStateBlob << player->keyState[sf::Keyboard::F];

		keyStateBlob << player->mouseState[0];
		keyStateBlob << player->mouseState[1];
		keyStateBlob << player->mousePosition[0];
		keyStateBlob << player->mousePosition[1];

		network->send(keyStateBlob);
	}

	player->update(delta);

	if(byteCounterClock.getElapsedTime().asSeconds() > 0.25){
		byteCounterClock.restart();
		network->DISP_TX = network->LAST_TX * 4;
		network->DISP_RX = network->LAST_RX * 4;
		network->LAST_RX = 0;
		network->LAST_TX = 0;
	}
}

void Game::render(float delta){
	(void) delta;
	window->clear();

	playerView->setCenter(player->x, player->y);
	window->setView(*playerView);

	// draw entities
	player->draw();
	for(player_t *agent : agents) agent->draw();
	for(zombie_t *zed : zombies) zed->draw();


	world->drawElements(window);


	// draw debug text
	latencyDisplayText.setString("Latency: " + std::to_string((int)(network->latency*1000)) + "ms\n"
		+ "Download: " + std::to_string(network->DISP_RX/1000.0) + "kB/s\n"
		+ "Upload:   " + std::to_string(network->DISP_TX/1000.0) + "kB/s\n"
		+ std::to_string((int) (1.0/delta)) + " fps\n"
	);

	latencyDisplayText.setPosition(
			playerView->getCenter().x - playerView->getSize().x/2,
			playerView->getCenter().y - playerView->getSize().y/2
	);

	window->draw(latencyDisplayText);



	window->display();
	// std::this_thread::sleep_for(std::chrono::milliseconds(10));
}
