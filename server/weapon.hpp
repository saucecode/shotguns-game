#ifndef _SAUCECODE_SHOTGUN_SERVER_WEAPON_HPP
#define _SAUCECODE_SHOTGUN_SERVER_WEAPON_HPP

#include <SFML/Graphics.hpp>

#include <string>
#include <cstdint>

typedef struct {
	std::string name;
	uint16_t id;
	bool isMelee;
	float damage;
	int32_t bulletCount;
	float shootDelay;
	float range;
} weapon_t;

namespace Weapon {           //      NAME     ID   MELEE  DMG  BUL  DLY  RANGE
	static const weapon_t shotgun = {"shotgun", 0, false, 160, 5, 0.8,   180};
	static const weapon_t p250 =    {"p250", 1, false,    100, 1, 0.2,   300};
};

class player_t;

class projectile_t {
public:

	sf::Vector2f start;
	sf::Vector2f end;
	float range;
	float life = 1;
	int16_t ownerid;
	sf::Vertex line[2];

	projectile_t(int16_t ownerid, sf::Vector2f start, sf::Vector2f end, float range);
	projectile_t(int16_t ownerid, sf::Vector2f start, sf::Vector2f end);
	projectile_t(int16_t ownerid, float x, float y, float angle, float range);
	projectile_t(int16_t ownerid, sf::Vector2f start, float angle, float range);

	void constructVertices();

};

#endif
