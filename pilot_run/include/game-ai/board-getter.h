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

		for (auto const& path_node : this->GetPathNodes(leaf)) {
			board.ApplyMove(path_node->move);
		}

#ifdef DEBUG
		size_t current_hash = std::hash<GameEngine::Board>()(board);
		if (this->board_hash != current_hash) {
			throw std::runtime_error("consistency check failed: board hash changed");
		}
#endif
	}

	std::list<TreeNode const*> GetPathNodes(TreeNode const* leaf_node) const
	{
		std::list<TreeNode const*> path_nodes;
		auto path_iterator = this->path.GetReverseIterator(leaf_node);

		while (true) {
			auto const& node = path_iterator.GetNodeAndMoveUpward();
			if (node == nullptr) break;
			if (node->parent == nullptr) break;
			path_nodes.push_front(node);
		}

		return path_nodes;
	}

	BoardWithMoves * GetCorrespondsBoardInitializer(TreeNode const* leaf) const
	{
		std::list<GameEngine::Move> moves;
		for (auto const& path_node : this->GetPathNodes(leaf)) {
			moves.push_back(path_node->move);
		}
		return new BoardWithMoves(this->start_board_random, std::unique_ptr<BoardInitializer>(this->start_board->Clone()), std::move(moves));
	}

#ifdef DEBUG
	void SetBoardHash(size_t hash) { this->board_hash = hash; }
	size_t GetBoardHash() const { return this->board_hash; }
#endif

private:
	BoardInitializer * start_board;
	int start_board_random; // (any) one of the start board randoms which leads us to this node
	TraversedPathRecorder path; // one of the paths leading to this node

#ifdef DEBUG
	size_t board_hash;
#endif
};