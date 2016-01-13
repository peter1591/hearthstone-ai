#ifndef _GAME_H
#define _GAME_H

#include <list>

#include "player.h"
#include "opponent.h"

// This is the input of the AI, which records all the information
// we can get from the game history.
struct Game
{
	Player player;
	Opponent opponent;
};

#endif

