#ifndef GAME_AI_MCTS_H
#define GAME_AI_MCTS_H

#include <random>
#include <list>
#include <map>
#include <unordered_set>
#include "game-engine/board.h"
#include "tree.h"
#include "board-node-map.h"

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

private:
	void Select(TreeNode* const& node, GameEngine::Board const& board, TreeNode* & new_node, GameEngine::Board & new_board);
	bool Expand(TreeNode *node, GameEngine::Board const& board, TreeNode* & new_node, GameEngine::Board & new_board);
	bool Simulate(GameEngine::Board &board);
	void BackPropagate(TreeNode *node, bool is_win);

	void GetNextState(TreeNode *node, GameEngine::Move &move, GameEngine::Board &board);
	TreeNode* IsBoardTraversed(TreeNode *parent, GameEngine::Board const& parent_board, const GameEngine::Board new_child_board);
	int GetRandom();

public:
	GameEngine::Board root_node_board;
	Tree tree;
	BoardNodeMap board_node_map;

private:
	TreeNode *allocated_node;
};

#endif
