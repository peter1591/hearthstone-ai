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
	void SelectAndExpand(TreeNode* & node, GameEngine::Board & board);
	TreeNode* CreateChildNode(TreeNode* const node, GameEngine::Move const& next_move, GameEngine::Board & next_board);
	bool Simulate(GameEngine::Board &board);
	void BackPropagate(bool is_win);

	int GetRandom();
	TreeNode * CreateRedirectNode(TreeNode * parent, GameEngine::Move const& move, TreeNode * target_node);
	bool ExpandNewNode(TreeNode * & node, GameEngine::Board & board);
	bool ExpandNodeWithDeterministicMoves(TreeNode * & node, GameEngine::Board & board);
	bool ExpandNodeWithRandomMoves(TreeNode * & node, GameEngine::Board & board);

private: // for internal use
	TreeNode *allocated_node;

	std::vector<TreeNode*> traversed_nodes;
};

#endif
