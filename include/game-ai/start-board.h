#pragma once

#include "game-engine/board.h"

class StartBoard
{
public:
	StartBoard();

	GameEngine::Board GetBoard(int rand_seed);

private: // for debug only
	static void InitializeDebugDeck1(GameEngine::Deck &deck);
	static void InitializeDebugHand1(GameEngine::Hand &hand);
	static void InitializeDebugBoard1(GameEngine::Board & board);

	GameEngine::Board board_debug1;
};