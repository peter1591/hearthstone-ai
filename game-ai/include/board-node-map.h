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

public: // used to copy/merge tree
	void UpdateNodePointers(const std::unordered_map<TreeNode*, TreeNode*>& node_map);
	void FillHash(std::map<TreeNode*, std::size_t> &nodes) const;

	void Clear();

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
	this->map[board_hash].insert(node);
	//this->parent_map[node->parent][board_hash].insert(node);
}