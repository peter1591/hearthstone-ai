#ifndef GAME_AI_MCTS_H
#define GAME_AI_MCTS_H

#include <random>
#include <list>
#include <map>
#include <unordered_set>
#include "game-engine/game-engine.h"
#include "tree.h"
#include "board-node-map.h"

class MCTS
{
public:
	MCTS();
	~MCTS();
	MCTS(const MCTS&) = delete;
	MCTS &operator=(const MCTS&) = delete;
	MCTS(MCTS&&);
	MCTS &operator=(MCTS&&);

	bool operator==(const MCTS&) const;
	bool operator!=(const MCTS&) const;

public:
	void Initialize(unsigned int rand_seed, const GameEngine::Board &starting_board);
	void Iterate();

public:
	GameEngine::Board root_node_board;
	Tree tree;
	BoardNodeMap board_node_map;

private:
	bool Select(TreeNode* & node, GameEngine::Board & board, GameEngine::Move & expanding_move);
	bool Expand(TreeNode* & node, GameEngine::Board & board, GameEngine::Move const& expanding_move);
	bool Simulate(GameEngine::Board &board);
	void BackPropagate(bool is_win);

	void GetNextMove(TreeNode *node, GameEngine::Board const& board, GameEngine::Move &next_move);
	int GetRandom();
	TreeNode * FindDuplicateNode(TreeNode * node, GameEngine::Move const& next_move, GameEngine::Board const& next_board, bool introduced_random);
	TreeNode * CreateRedirectNode(TreeNode * parent, GameEngine::Move const& move, TreeNode * target_node);
	TreeNode * FindBestChildToExpand(TreeNode * parent, GameEngine::Board const& parent_board, GameEngine::Move & best_move);

private: // for internal use
	TreeNode *allocated_node;

	std::vector<TreeNode*> traversed_nodes;
};

#endif
