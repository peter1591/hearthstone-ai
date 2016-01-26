#ifndef GAME_AI_MCTS_H
#define GAME_AI_MCTS_H

#include <random>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include "game-engine/board.h"
#include "tree.h"

class MCTS
{
	public:
		MCTS();
		MCTS(const MCTS&);
		MCTS &operator=(const MCTS&);
		MCTS(MCTS&&);
		MCTS &operator=(MCTS&&);

		bool operator==(const MCTS&) const;
		bool operator!=(const MCTS&) const;

	public:
		void Initialize(unsigned int rand_seed, const GameEngine::Board &starting_board);

		void Iterate();

		void Merge(const MCTS&& rhs);

		void DebugPrint();

	private:
		class BoardNodesMapping
		{
		public:
			bool operator==(const BoardNodesMapping &rhs) const;
			bool operator!=(const BoardNodesMapping &rhs) const;

		public:
			void Add(const GameEngine::Board &board, TreeNode *node, const MCTS& mcts);
			void UpdateNodePointers(const std::unordered_map<TreeNode*, TreeNode*>& node_map);

			std::unordered_set<TreeNode *> Find(const GameEngine::Board &board, const MCTS& mcts);

		private:
			std::unordered_map<std::size_t, std::unordered_set<TreeNode *> > data; // hash of board --> tree nodes
		};

	private:
		int GetRandom();

		// Find a node to expand, and expand it
		// @param board [OUT] the new board of the node
		// @return the new node
		TreeNode * SelectAndExpand(GameEngine::Board &board);
		TreeNode * Select(TreeNode *starting_node, GameEngine::Board &board);
		void Expand(TreeNode *node, GameEngine::Move &move, GameEngine::Board &board);

		bool Simulate(GameEngine::Board &board);
		void SimulateWithBoard(GameEngine::Board &board);

		void BackPropagate(TreeNode *node, bool is_win);

		void MergeMCTSNodes(const TreeNode *source, TreeNode *target);
		void MergeMCTSNodeToParent(const TreeNode *source, TreeNode *parent,
			const std::unordered_map<GameEngine::Move, TreeNode*> &parent_move_map,
			std::unordered_set<GameEngine::Move> &parent_rest_moves);

		void PrintBestRoute(int levels);
		void PrintTree(TreeNode *node, int level, const int max_level);

	private:
		GameEngine::Board root_node_board;
		Tree tree;
		BoardNodesMapping board_nodes_mapping;
		std::mt19937 random_generator;
};

#endif
