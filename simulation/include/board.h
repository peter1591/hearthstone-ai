#ifndef _BOARD_H
#define _BOARD_H

#include "player.h"
#include "opponent.h"

// POD to support fast copy via memcpy()
struct Board
{
	Player player;
	Opponent opponent;

	enum Turn {
		TURN_PLAYER, TURN_OPPONENT, TURN_RNG
	};
	Turn turn;
};

#endif
