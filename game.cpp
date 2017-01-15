
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include <iostream>
#include <thread>

#include "server/weapon.hpp"

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
	player = new player_t(this, -1, 0,0, "snowflake");
	network = new Network(this, "127.0.0.1", 43234);

	window = new sf::RenderWindow(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "SFML works!");
	// window->setMouseCursorVisible(false);
	playerView = new sf::View(sf::FloatRect(0,0,WINDOW_WIDTH, WINDOW_HEIGHT));
	playerView->setCenter(player->x, player->y);

	window->setView(*playerView);
	//window->setVerticalSyncEnabled(true);
	window->setFramerateLimit(60);

	latencyDisplayText.setFont(*(resourceManager->fonts.at("primary")));
	latencyDisplayText.setString("Latency: ?");
	latencyDisplayText.setFillColor(sf::Color::White);
	latencyDisplayText.setCharacterSize(16);

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

	player->mouseState[0] = false;
	player->mouseState[1] = false;

	if(focused){
		for(int i=0; i<256; i++){
			player->keyState[i] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key(i));
		}

		player->mouseState[0] = sf::Mouse::isButtonPressed(sf::Mouse::Left);
		player->mouseState[1] = sf::Mouse::isButtonPressed(sf::Mouse::Right);

		sf::Vector2i mousePosition = sf::Mouse::getPosition(*window);
		player->mousePosition[0] = (short) (mousePosition.x + (int) player->x - WINDOW_WIDTH/2);
		player->mousePosition[1] = (short) (mousePosition.y + (int) player->y - WINDOW_HEIGHT/2);
	}

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

	player->update(delta);

	// clear dead zombies
	zombiesMutex.lock();
	zombies.erase(
		std::remove_if(zombies.begin(), zombies.end(),
			[](zombie_t *zed){
				return zed->health <= 0.0;
			}),
		zombies.end()
	);
	zombiesMutex.unlock();


	if(byteCounterClock.getElapsedTime().asSeconds() > 0.25){
		float timeElapsed = byteCounterClock.restart().asSeconds();
		network->DISP_TX = network->LAST_TX / timeElapsed;
		network->DISP_RX = network->LAST_RX / timeElapsed;
		network->LAST_RX = 0;
		network->LAST_TX = 0;
	}
}

void Game::render(float delta){
	window->clear();

	playerView->setCenter(player->x, player->y);
	window->setView(*playerView);

	// draw entities
	player->draw();
	for(player_t *agent : agents) agent->draw();
	for(zombie_t *zed : zombies) zed->draw();


	world->drawElements(window);

	// draw projectiles
	projectilesMutex.lock();
	for(projectile_t *projectile : projectiles){
		if(projectile->life > 0.1)
			projectile->life -= delta;
		else
			projectile->life = 0;

		projectile->line[0].color.a = (unsigned char) (0xff * projectile->life * 0.5);
		projectile->line[1].color.a = projectile->line[0].color.a * 2;
		window->draw(projectile->line, 2, sf::Lines);
	}

	// clear dead projectiles
	projectiles.erase(
		std::remove_if(projectiles.begin(), projectiles.end(),
			[](projectile_t *p){
				return p->life <= 0.0;
			}),
		projectiles.end()
	);

	projectilesMutex.unlock();


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
