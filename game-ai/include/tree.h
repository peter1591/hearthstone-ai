#ifndef GAME_AI_TREE_H
#define GAME_AI_TREE_H

#include <vector>
#include "game-engine/board.h"

class TreeNode
{
	public:
		typedef std::vector<TreeNode*> children_type;

	public:
		TreeNode() : wins(0), count(0) {}

		void AddChild(TreeNode *node) {
			this->children.push_back(node);
			node->parent = this;
		}

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

		// is this a GAME_FLOW node, and introduces some randoms?
		// only valid if this node is expanded (i.e., has children)
		bool has_random_playouts;

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

#endif
