#ifndef _SAUCECODE_SHOTGUN_SERVER_WEAPON_HPP
#define _SAUCECODE_SHOTGUN_SERVER_WEAPON_HPP

typedef struct {
	unsigned short id;
	bool isMelee;
	float damage;
	int bulletCount;
	float shootDelay;
} weapon_t;

namespace Weapon {
	const weapon_t shotgun = {0, false, 16, 5, 0.8};
};

#endif
