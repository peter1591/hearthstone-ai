#pragma once

#include "mcts.h"
#include "board-node-map.h"

inline TreeNode * BoardNodeMap::Find(const GameEngine::Board &board, const MCTS& mcts) const
{
	std::size_t hash = std::hash<GameEngine::Board>()(board);

	auto it_found = this->map.find(hash);
	if (it_found == this->map.end()) return nullptr;

	for (const auto &possible_node : it_found->second) {
		GameEngine::Board it_board;
		possible_node->GetBoard(mcts.root_node_board, it_board);
		if (board == it_board) return possible_node;

#ifdef DEBUG
		std::cout << "hash collision!" << std::endl;
#endif
	}

	return nullptr;
}