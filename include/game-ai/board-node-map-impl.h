#pragma once

#include "mcts.h"
#include "board-node-map.h"

inline bool BoardNodeMap::operator==(const BoardNodeMap &rhs) const
{
	if (this->map != rhs.map) return false;
	return true;
}

inline bool BoardNodeMap::operator!=(const BoardNodeMap &rhs) const
{
	return !(*this == rhs);
}

inline void BoardNodeMap::Add(const GameEngine::Board &board, TreeNode *node)
{
	std::size_t hash = std::hash<GameEngine::Board>()(board);
	this->Add(hash, node);
}

inline void BoardNodeMap::Add(std::size_t board_hash, TreeNode *node)
{
#ifdef DEBUG
	if (this->map.find(board_hash) != this->map.end())
	{
		auto const& exist_node = *this->map[board_hash].begin();

		std::cerr << "hash collision: " << board_hash << std::endl;

		std::function<void(TreeNode*)> print_backtrace;
		print_backtrace = [&print_backtrace](TreeNode * node) {
			if (node == nullptr) return;
			print_backtrace(node->parent);
			if (node->parent != nullptr) std::cerr << "Move: " << node->move.GetDebugString() << std::endl;
			else std::cerr << "(from root board)" << std::endl;
		};

		std::cerr << "Existing board comes from moves: " << std::endl;
		print_backtrace(exist_node);

		std::cerr << "===============================" << std::endl;
		std::cerr << "New board comes from moves: " << std::endl;
		print_backtrace(node);
	}
#endif

	this->map[board_hash].insert(node);
}

inline TreeNode * BoardNodeMap::Find(const GameEngine::Board &board, BoardInitializer * board_initializer) const
{
	std::size_t hash = std::hash<GameEngine::Board>()(board);

	auto it_found = this->map.find(hash);
	if (it_found == this->map.end()) return nullptr;

	for (const auto &possible_node : it_found->second) {
		GameEngine::Board it_board;
		board_initializer->InitializeBoard(possible_node->board_getter->GetStartBoardRandom(), it_board);
		possible_node->GetBoard(it_board);
		if (board == it_board) return possible_node;

#ifdef DEBUG
		std::cerr << "hash collision!" << std::endl;
#endif
	}

	return nullptr;
}