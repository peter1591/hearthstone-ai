#ifndef _BOARD_H
#define _BOARD_H

#include "player.h"
#include "opponent.h"

// POD to support fast copy via memcpy()
class Board
{
	public:
		enum Turn {
			TURN_PLAYER, TURN_OPPONENT, TURN_RNG
		};

	public:
		Player player;
		Opponent opponent;

		Turn turn;
};

#endif
