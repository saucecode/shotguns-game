
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

#include "resource_manager.hpp"

ResourceManager::ResourceManager(){
	std::cout << "ResourceManager initialized.\n";

}


void ResourceManager::loadResourcesFolder(std::string directory){

	std::cout << "\nLoading resources from directory " << directory << " ...\n\n";

	std::ifstream manifestFile(directory + "/RESOURCES.MANIFEST");
	std::string line;

	while(std::getline(manifestFile, line)){
		std::stringstream lineStream(line);
		std::string data, type, name, filename;

		std::getline(lineStream, type, ' ');
		lineStream >> name >> filename;

		if(type[0] == '#') continue; // ignore comments
		if(type.empty()) continue;   // ignore empty lines

		std::cout << "Read: " << type << ", " << name << ", " << filename << "\n";

		if(type == "texture"){
			doLoadTexture(name, directory + "/" + filename);
		}else if(type == "font"){
			doLoadFont(name, directory + "/" + filename);
		}
	}

	manifestFile.close();
	std::cout << "loadResourcesFolder exiting...\n\n";
}

void ResourceManager::doLoadTexture(std::string name, std::string filepath){
	sf::Texture *tex = new sf::Texture();
	if(tex->loadFromFile(filepath)){
		this->textures.emplace(name, tex);
		std::cout << "Loaded/Indexed " << filepath << " into " << name << "\n";
	}
}

void ResourceManager::doLoadFont(std::string name, std::string filepath){
	sf::Font *font = new sf::Font();
	if(font->loadFromFile(filepath)){
		this->fonts.emplace(name, font);
		std::cout << "Loaded/Indexed " << filepath << " into " << name << "\n";
	}
}
