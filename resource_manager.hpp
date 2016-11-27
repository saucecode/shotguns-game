#ifndef _SAUCECODE_SHOTGUN_RESOURCE_MANAGER_HPP
#define _SAUCECODE_SHOTGUN_RESOURCE_MANAGER_HPP

#include <SFML/Graphics.hpp>

#include <string>
#include <vector>

class ResourceManager {
public:

	std::map<std::string, sf::Texture*> textures;
	std::map<std::string, sf::Font*> fonts;

	ResourceManager();
	void loadResourcesFolder(std::string directory);

	void doLoadTexture(std::string name, std::string filepath);
	void doLoadFont(std::string name, std::string filepath);

};

#endif
