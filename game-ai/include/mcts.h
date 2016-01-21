#ifndef GAME_AI_MCTS_H
#define GAME_AI_MCTS_H

#include "tree.h"

class MCTS
{
	public:
		void Initialize(const GameEngine::Board &board);

		TreeNode* Select();

		TreeNode* Expand(TreeNode *node);

		bool Simulate(TreeNode *node);

		void BackPropagate(TreeNode *node, bool is_win);

		void DebugPrint();

	private:
		void PrintBestRoute();
		void PrintTree(TreeNode *node, int level, const int max_level);

		Tree tree;
};

#endif
