#pragma once

#include "game-engine/board.h"

class StartBoard
{
public:
	GameEngine::Board GetBoard(unsigned int rand_seed);

private: // for debug only
	static void InitializeDebugDeck1(GameEngine::Deck &deck);
	static void InitializeDebugHand1(GameEngine::Hand &hand);
	static void InitializeDebugBoard1(GameEngine::Board & board);
};