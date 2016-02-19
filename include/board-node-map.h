#pragma once

#include <map>
#include <unordered_set>
#include "game-engine/board.h"
#include "tree.h"

class MCTS;

class BoardNodeMap
{
public:
	bool operator==(const BoardNodeMap &rhs) const;
	bool operator!=(const BoardNodeMap &rhs) const;

public:
	void Add(const GameEngine::Board &board, TreeNode *node);
	void Add(std::size_t board_hash, TreeNode *node);
	TreeNode * Find(const GameEngine::Board &board, const MCTS& mcts) const;

private:
	typedef std::unordered_map<std::size_t, std::unordered_set<TreeNode*> > MapBoardToNodes;

private:
	MapBoardToNodes map; // hash of board --> tree nodes
};

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

		std::cout << "hash collision: " << board_hash << std::endl;

		std::function<void(TreeNode*)> print_backtrace;
		print_backtrace = [&print_backtrace](TreeNode * node) {
			if (node == nullptr) return;
			print_backtrace(node->parent);
			if (node->parent != nullptr) std::cout << "Move: " << node->move.GetDebugString() << std::endl;
			else std::cout << "(from root board)" << std::endl;
		};

		std::cout << "Existing board comes from moves: " << std::endl;
		print_backtrace(exist_node);

		std::cout << "===============================" << std::endl;
		std::cout << "New board comes from moves: " << std::endl;
		print_backtrace(node);
	}
#endif

	this->map[board_hash].insert(node);
}