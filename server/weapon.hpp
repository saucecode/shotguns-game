#ifndef _SAUCECODE_SHOTGUN_SERVER_WEAPON_HPP
#define _SAUCECODE_SHOTGUN_SERVER_WEAPON_HPP

#include <SFML/Graphics.hpp>

#include <string>

typedef struct {
	std::string name;
	unsigned short id;
	bool isMelee;
	float damage;
	int bulletCount;
	float shootDelay;
	float range;
} weapon_t;

namespace Weapon {
	static const weapon_t shotgun = {"shotgun", 0, false, 160, 5, 0.8, 180};
	static const weapon_t p250 =    {"p250", 0, false, 100, 1, 0.2, 300};
};

class player_t;

class projectile_t {
public:

	sf::Vector2f start;
	sf::Vector2f end;
	float range;
	float life = 1;
	sf::Vertex line[2];

	player_t *owner;

	projectile_t(player_t *owner, sf::Vector2f start, sf::Vector2f end, float range);
	projectile_t(player_t *owner, sf::Vector2f start, sf::Vector2f end);
	projectile_t(player_t *owner, float x, float y, float angle, float range);
	projectile_t(player_t *owner, sf::Vector2f start, float angle, float range);

	void constructVertices();

};

#endif
