#ifndef GAME_AI_TREE_H
#define GAME_AI_TREE_H

#include <queue>
#include <functional>
#include <vector>
#include <unordered_map>
#include <set>
#include "game-engine/board.h"

class TreeNode
{
public:
	typedef std::list<TreeNode*> children_type;

public:
	TreeNode() : parent(nullptr), equivalent_node(nullptr) {}

	void AddChild(TreeNode *node);
	void GetBoard(const GameEngine::Board &root_node_board, GameEngine::Board &board) const;

public:
	TreeNode *parent;
	children_type children;

	TreeNode *equivalent_node; // the board is the same as this node, only move/count are valid info for this node

public:
	GameEngine::Stage stage;
	GameEngine::StageType stage_type;

	// what move lead us from parent to this state?
	GameEngine::Move move;

	// if this is a player's turn, then it's true
	// if this is a random node, and it came out due to a player's action, then it's true
	// otherwise, it's false
	bool is_player_node;

	// if it's player's turn, this is the number of WINNINGS in all children
	// if it's opponent's turn, this is the number of LOSSES in all children
	int wins;

	// number of simulations
	int count;

	// only valid if this node is expanded (i.e., has children)
	GameEngine::NextMoveGetter next_move_getter; // not valid for game-flow nodes
	bool next_moves_are_random; // The next moves might differ from time to time. E.g., game-flow moves
};

class Tree
{
public:
	Tree();
	~Tree();

	Tree(const Tree& rhs) = delete;
	Tree & operator=(const Tree& rhs) = delete;

	Tree(Tree&& rhs) = delete;
	Tree & operator=(Tree&& rhs) = delete;

public:
	void CreateRootNode();
	TreeNode * GetRootNode() const;

private:
	static void ClearSubtree(TreeNode *node);

private:
	TreeNode * root_node;

};

inline void TreeNode::AddChild(TreeNode *node)
{
	this->children.push_back(node);
	node->parent = this;
}

inline void TreeNode::GetBoard(const GameEngine::Board &root_node_board, GameEngine::Board &board) const
{
	if (this->parent == nullptr) {
		board = GameEngine::Board::Clone(root_node_board);
		return;
	}

	this->parent->GetBoard(root_node_board, board);
	board.ApplyMove(this->move);
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

inline TreeNode * Tree::GetRootNode() const
{
	return this->root_node;
}

inline void Tree::ClearSubtree(TreeNode *node)
{
	for (const auto &child : node->children)
	{
		Tree::ClearSubtree(child);
	}
	delete node;
}

#endif
