#ifndef _BOARD_H
#define _BOARD_H

#include <list>

#include "player.h"
#include "opponent.h"

// POD to support fast copy via memcpy()

// The hidden information of opponent's secrets/hands/decks
// will be determined when starting a MCTS iteration
// So, there are no hidden info here.

struct Board
{
	Player player;
	Player opponent;

	bool is_my_turn; // is the player playing? or the opponent playing?
};

#endif
