#pragma once

#include "game-engine/board.h"

class StartBoard
{
public:
	StartBoard();

	GameEngine::Board GetBoard(int rand_seed) const;

private: // for debug only
	static void InitializeDebugBoard1_PlayerHand(GameEngine::Hand &hand);
	static void InitializeDebugBoard1_OpponentHand(GameEngine::Hand &hand);

	static void InitializeDebugBoard1(GameEngine::Board & board);
	static void InitializeDebugBoard2(GameEngine::Board & board);

	static void StartBoard::InitializeDebugBoard3_Hand(GameEngine::Hand &hand);
	static void InitializeDebugBoard3(GameEngine::Board & board);

	static void InitializeDeck_Practice_Mage(GameEngine::Hand & hand);
	static void InitializeDebugBoard_Practice_Mage(GameEngine::Board & board);

	GameEngine::Board board_debug1;
};