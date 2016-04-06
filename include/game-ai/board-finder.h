#pragma once

#include "game-engine/board.h"
#include "board-node-map.h"

// Find a particular board among the MCTS tree
class BoardFinder
{
public:
	void Add(const GameEngine::Board &board, TreeNode *node)
	{
		return this->map.Add(board, node);
	}

	TreeNode * Find(const GameEngine::Board &board, StartBoard const& start_board) const
	{
		return this->map.Find(board, start_board);
	}

private:
	BoardNodeMap map;
};