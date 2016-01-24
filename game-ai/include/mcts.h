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
		class TraversedBoards
		{
		public:
			void Add(const GameEngine::Board &board, TreeNode *node, const MCTS& mcts);

			TreeNode * Find(const GameEngine::Board &board, const MCTS& mcts);

		private:
			std::unordered_multimap<std::size_t, TreeNode *> data; // hash of board --> tree node
		};

	private:
		TreeNode * Select(TreeNode *starting_node, GameEngine::Board &board);
		void Expand(TreeNode *node, GameEngine::Move &move, GameEngine::Board &board);

		void SimulateWithBoard(GameEngine::Board &board);

		void PrintBestRoute(int levels);
		void PrintTree(TreeNode *node, int level, const int max_level);

		GameEngine::Board root_node_board;
		Tree tree;
		TraversedBoards traversed_boards;
		unsigned int rand_seed;
};

#endif
