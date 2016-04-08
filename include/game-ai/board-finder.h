#pragma once

#include <vector>
#include "game-engine/board.h"
#include "board-node-map.h"

// Find a particular board among the MCTS tree
// Note: a board is only found within the nodes under the same turn
//       since most (if not all) of the transposition nodes occurs in the same turn
class BoardFinder
{
public:
	BoardFinder()
	{
		this->turn_maps.reserve(100);
	}

	void Add(const GameEngine::Board &board, TreeNode *node)
	{
		int turn = node->turn;

		for (int count = (turn - (int)this->turn_maps.size()); count >= 0; --count)
		{
			this->turn_maps.push_back(BoardNodeMap());
		}

		return this->turn_maps[turn].Add(board, node);
	}

	TreeNode * Find(int turn, const GameEngine::Board &board, BoardInitializer * board_initializer) const
	{
		if (turn >= this->turn_maps.size()) {
			return nullptr;
		}

		return this->turn_maps[turn].Find(board, board_initializer);
	}

private:
	std::vector<BoardNodeMap> turn_maps;
};