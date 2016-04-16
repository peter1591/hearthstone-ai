#pragma once

#include "game-engine/board.h"
#include "board-initializer.h"

class StartBoard : public BoardInitializer
{
public:
	void InitializeBoard(int rand_seed, GameEngine::Board & board) const;

	BoardInitializer * Clone() const;

private: // for debug only
	static void InitializeDebugBoard1_PlayerHand(GameEngine::Hand &hand);
	static void InitializeDebugBoard1_OpponentHand(GameEngine::Hand &hand);

	static void InitializeDebugBoard1(GameEngine::Board & board);
	static void InitializeDebugBoard2(GameEngine::Board & board);

	static void InitializeDebugBoard3_Hand(GameEngine::Hand &hand);
	static void InitializeDebugBoard3(GameEngine::Board & board);

	static void InitializeDeck_Practice_Mage_Player(GameEngine::Hand & hand);
	static void InitializeDeck_Practice_Mage_Opponent(GameEngine::Hand & hand);
	static void InitializeDebugBoard_Practice_Mage(GameEngine::Board & board);
};