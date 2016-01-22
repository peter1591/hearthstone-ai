#ifndef GAME_AI_MCTS_H
#define GAME_AI_MCTS_H

#include "game-engine/board.h"
#include "tree.h"

class MCTS
{
	public:
		void Initialize(const GameEngine::Board &board);

		TreeNode* Select();

		TreeNode* Expand(TreeNode *node, GameEngine::Board &new_board);

		bool Simulate(GameEngine::Board &board);

		void BackPropagate(TreeNode *node, bool is_win);

		void DebugPrint();

	private:
		TreeNode *ExpandNodeWithMove(TreeNode *parent, const GameEngine::Move &move, GameEngine::Board &new_board);
		void GetBoardByApplyingMoves(TreeNode *parent, GameEngine::Board &board) const;
		void SimulateWithBoard(GameEngine::Board &board);

		void PrintBestRoute();
		void PrintTree(TreeNode *node, int level, const int max_level);

		GameEngine::Board root_node_board;
		Tree tree;
		unsigned int rand_seed;
};

#endif
