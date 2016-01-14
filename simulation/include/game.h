#ifndef _GAME_H
#define _GAME_H

#include <list>

#include "player.h"
#include "opponent.h"

// This is the input of the AI, which records all the information
// we can get from the game history.
// This information includes:
//   (1) The current game stats like hp/armor/etc.
//   (2) The played cards.
//   (3) The unknown secrets with some 'tested' information
struct Game
{
	Player player;
	Opponent opponent;
};

#endif

