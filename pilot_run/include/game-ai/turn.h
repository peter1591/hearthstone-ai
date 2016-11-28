#pragma once

#include "tree.h"

class Turn
{
public:
	static int GetTurnForRootNode() { return 1; }

	static int GetTurn(TreeNode * parent, GameEngine::Move const& move)
	{
		if (move.action == GameEngine::Move::ACTION_END_TURN) return parent->turn + 1;
		else return parent->turn;
	}
};