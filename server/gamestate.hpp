#ifndef _SAUCECODE_SHOTGUN_SERVER_GAMESTATE_HPP
#define _SAUCECODE_SHOTGUN_SERVER_GAMESTATE_HPP

#include <vector>

class player_t;
class zombie_t;
class world_t;

typedef struct {
	std::vector<player_t*> *players;
	std::vector<zombie_t*> *zombies;

	world_t *world;
} gamestate_t;

#endif
