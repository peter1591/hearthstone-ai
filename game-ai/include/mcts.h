#ifndef GAME_AI_MCTS_H
#define GAME_AI_MCTS_H

#include <unordered_map>
#include "game-engine/board.h"
#include "tree.h"

class MCTS
{
	public:
		void Initialize(const GameEngine::Board &board);

		// Find a node to expand, and expand it
		// @param board [OUT] the new board of the node
		// @return the new node
		TreeNode * SelectAndExpand(GameEngine::Board &board);

		bool Simulate(GameEngine::Board &board);

		void BackPropagate(TreeNode *node, bool is_win);

		void DebugPrint();

	private:
		void SimulateWithBoard(GameEngine::Board &board);

		void PrintBestRoute();
		void PrintTree(TreeNode *node, int level, const int max_level);

		GameEngine::Board root_node_board;
		Tree tree;
		std::unordered_map<GameEngine::Board, TreeNode *> traversed_boards;
		unsigned int rand_seed;
};

#endif
