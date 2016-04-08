#pragma once

#include "game-engine/board.h"

class BoardInitializer
{
public:
	virtual ~BoardInitializer() {}

	virtual GameEngine::Board GetBoard(int rand_seed) const = 0;
};