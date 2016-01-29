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

	std::unordered_set<TreeNode *> Find(const GameEngine::Board &board, const MCTS& mcts) const;
	TreeNode * FindUnderParent(const GameEngine::Board &board, TreeNode const* parent, GameEngine::Board const& parent_board) const;

public: // used to copy/merge tree
	void UpdateNodePointers(const std::unordered_map<TreeNode*, TreeNode*>& node_map);
	void FillHash(std::map<TreeNode*, std::size_t> &nodes) const;

	void Clear();

private:
	typedef std::unordered_map<std::size_t, std::unordered_set<TreeNode*> > MapBoardToNodes;

private:
	MapBoardToNodes map; // hash of board --> tree nodes

	std::unordered_map<TreeNode const*, MapBoardToNodes> parent_map; // parent node --> child node map
};