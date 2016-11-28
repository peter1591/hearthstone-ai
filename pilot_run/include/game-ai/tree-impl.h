#pragma once

#include "tree.h"
#include "board-getter.h"

inline void TreeNode::GetBoard(GameEngine::Board & board) const
{
	if (!this->board_getter) throw std::runtime_error("this tree node is not with an board getter!");

	this->board_getter->GetBoard(this, board);
}

inline TreeNode * TreeNode::FindChildByMove(GameEngine::Move const & child_move)
{
	if (this->move_child_map) {
		auto it = this->move_child_map->find(child_move);
		if (it == this->move_child_map->end()) return nullptr;
		return it->second;
	}

	// create the map on demand
	this->move_child_map.reset(new std::unordered_map<GameEngine::Move, TreeNode*>());

	TreeNode * ret = nullptr;
	for (auto const& child : this->children) {
		this->move_child_map->insert(std::make_pair(child->move, child));
		if (child->move == child_move) ret = child;
	}

	return ret;
}

inline Tree::Tree()
{
	this->root_node = nullptr;
}

inline Tree::~Tree()
{
	if (this->root_node) {
		Tree::ClearSubtree(this->root_node);
		this->root_node = nullptr;
	}
}

inline void Tree::CreateRootNode()
{
	if (this->root_node) throw std::runtime_error("root node already exists");
	this->root_node = new TreeNode;
}

inline TreeNode * Tree::GetRootNode()
{
	return this->root_node;
}

inline TreeNode const* Tree::GetRootNode() const
{
	return this->root_node;
}

inline void Tree::Clear()
{
	if (this->root_node) {
		this->ClearSubtree(this->root_node);
		this->root_node = nullptr;
	}
}

inline void Tree::ClearSubtree(TreeNode *node)
{
	for (const auto &child : node->children)
	{
		Tree::ClearSubtree(child);
	}
	delete node;
}
