#ifndef GAME_AI_TREE_H
#define GAME_AI_TREE_H

#include <vector>
#include <unordered_map>
#include <set>
#include "game-engine/board.h"

class TreeNode
{
	public:
		typedef std::list<TreeNode*> children_type;

	public:
		TreeNode() : wins(0), count(0) {}

		void AddChild(TreeNode *node);
		void GetBoard(const GameEngine::Board &root_node_board, GameEngine::Board &board) const;

	public:
		TreeNode *parent;
		children_type children;

		GameEngine::Stage stage;
		GameEngine::StageType stage_type;
#ifdef CHECK_MOVE_REAPPLIABLE
		GameEngine::Board board;
#endif

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
		std::vector<GameEngine::Move> moves_not_yet_expanded;
};

class Tree
{
	public:
		TreeNode & GetRootNode() { return this->root_node; }
		const TreeNode & GetRootNode() const { return this->root_node; }

	private:
		TreeNode root_node;
};

inline void TreeNode::AddChild(TreeNode *node)
{
	this->children.push_back(node);
	node->parent = this;
}

inline void TreeNode::GetBoard(const GameEngine::Board &root_node_board, GameEngine::Board &board) const
{
	if (this->parent == nullptr) {
		board = root_node_board;
		return;
	}

	this->parent->GetBoard(root_node_board, board);
	board.ApplyMove(this->move);

#ifdef CHECK_MOVE_REAPPLIABLE
	if (board != this->board) {
		throw std::runtime_error("node board not match");
	}
#endif
}

#endif
