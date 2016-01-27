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
	void Add(const GameEngine::Board &board, TreeNode *node, const MCTS& mcts);
	void Add(std::size_t board_hash, TreeNode *node);

	std::unordered_set<TreeNode *> Find(const GameEngine::Board &board, const MCTS& mcts);

public: // used to copy/merge tree
	void UpdateNodePointers(const std::unordered_map<TreeNode*, TreeNode*>& node_map);
	void FillHash(std::map<TreeNode*, std::size_t> &nodes) const;

	void Clear();
	void Erase(const std::map<TreeNode *, std::size_t> &erasing_nodes);

private:
	std::unordered_map<std::size_t, std::unordered_set<TreeNode *> > data; // hash of board --> tree nodes
};