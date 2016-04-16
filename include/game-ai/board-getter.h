#pragma once

#include <list>
#include "tree.h"
#include "traversed-path-recorder.h"
#include "game-engine/board.h"
#include "board-initializer/board-initializer.h"

// Get a board for the corresponding tree node
class BoardGetter
{
public:
	BoardGetter(BoardInitializer * start_board, int start_board_random, TraversedPathRecorder const& path)
		: start_board(start_board), start_board_random(start_board_random), path(path)
	{
	}

	void GetBoard(TreeNode const* leaf, GameEngine::Board & board) const
	{
		this->start_board->InitializeBoard(this->start_board_random, board);

		std::list<TreeNode const*> path_nodes;
		auto path_iterator = this->path.GetReverseIterator(leaf);

		while (true) {
			auto const& node = path_iterator.GetNodeAndMoveUpward();
			if (node == nullptr) break;
			if (node->parent == nullptr) break;
			path_nodes.push_front(node);
		}

		for (auto const& path_node : path_nodes) {
			board.ApplyMove(path_node->move);
		}

#ifdef DEBUG
		size_t current_hash = std::hash<GameEngine::Board>()(board);
		if (this->board_hash != current_hash) {
			throw std::runtime_error("consistency check failed: board hash changed");
		}
#endif
	}

private:
	BoardInitializer * start_board;
	int start_board_random; // (any) one of the start board randoms which leads us to this node
	TraversedPathRecorder path; // one of the paths leading to this node
};