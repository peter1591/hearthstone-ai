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
	// (Normal) node must be unique across the entire tree
	void Add(const GameEngine::Board &board, TreeNode *node);
	void Add(std::size_t board_hash, TreeNode *node);
	TreeNode * FindWithoutRedirectNode(const GameEngine::Board &board, const MCTS& mcts) const;

	// Redirect nodes should be unique within a parent's children
	void AddRedirectNode(GameEngine::Board const& board, TreeNode *node);
	void AddRedirectNode(std::size_t board_hash, TreeNode *node);
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

inline bool BoardNodeMap::operator==(const BoardNodeMap &rhs) const
{
	if (this->map != rhs.map) return false;
	if (this->parent_map != rhs.parent_map) return false;
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
	this->parent_map[node->parent][board_hash].insert(node);
}

inline void BoardNodeMap::AddRedirectNode(const GameEngine::Board &board, TreeNode *node)
{
	std::size_t hash = std::hash<GameEngine::Board>()(board);
	this->AddRedirectNode(hash, node);
}

inline void BoardNodeMap::AddRedirectNode(std::size_t board_hash, TreeNode *node)
{
	this->parent_map[node->parent][board_hash].insert(node);
}