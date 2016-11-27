#ifndef _SAUCECODE_SHOTGUN_PACKETID_HPP
#define _SAUCECODE_SHOTGUN_PACKETID_HPP

const unsigned char PACKET_CONNECT = 0,
	PACKET_DISCONNECT = 1,
	PACKET_ADD_PLAYER = 2,
	PACKET_DROP_PLAYER = 3,
	PACKET_MOVE_PLAYER = 4,
	PACKET_WORLD_DATA = 5,
	PACKET_KEY_STATE = 6,
	PACKET_ADD_ZOMBIE = 7,
	PACKET_DROP_ZOMBIE = 8,
	PACKET_MOVE_ZOMBIE = 9,
	PACKET_PINGAZ = 10,
	PACKET_SPAWN_PROJECTILE = 11;

/*
	KEY STATE MAPPING
	0  W
	1  A
	2  S
	3  D
	4  <space>
	5  Q
	6  E
	7  F
	8  <LMB>
	9  <RMB>
	...
	11 ??!?
*/

#endif
