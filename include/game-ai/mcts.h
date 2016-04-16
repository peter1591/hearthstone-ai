#ifndef GAME_AI_MCTS_H
#define GAME_AI_MCTS_H

#include <fstream>
#include <random>
#include <list>
#include <map>
#include <unordered_set>
#include <json/json.h>
#include "game-engine/game-engine.h"
#include "game-ai/tree.h"
#include "game-ai/board-node-map.h"
#include "game-ai/traversed-path-recorder.h"
#include "game-ai/board-finder.h"
#include "game-ai/board-initializer/board-initializer.h"

class MCTS
{
public:
	MCTS();
	~MCTS();
	MCTS(const MCTS&) = delete;
	MCTS &operator=(const MCTS&) = delete;
	MCTS(MCTS&&) = delete;
	MCTS &operator=(MCTS&&) = delete;

public: // Operations
	void Initialize(unsigned int rand_seed, std::unique_ptr<BoardInitializer> && board_initializer);
	void UpdateRoot(Json::Value const& json_board);
	void UpdateRoot(std::unique_ptr<BoardInitializer> && board_initializer);

	void Iterate();

public:
	void GetDecideInformation(Tree const* &tree, TreeNode const* & root_node) const;

private:
	void ChangeBoardInitializer(std::unique_ptr<BoardInitializer> && new_initializer, TreeNode * node = nullptr);

	void CreateRootNode(GameEngine::Board const& board);
	void SelectAndExpand(TreeNode* & node, GameEngine::Board & board);
	bool Simulate(GameEngine::Board &board);
	void BackPropagate(bool is_win);

	// return true if the stage uses next-move-getter to get next moves
	bool UseNextMoveGetter(TreeNode * node);

	unsigned int GetRandom();
	TreeNode* CreateChildNode(TreeNode* const node, GameEngine::Move const& next_move, GameEngine::Board & next_board);
	TreeNode* CreateRedirectNode(TreeNode * parent, GameEngine::Move const& move, TreeNode * target_node);

	TreeNode * FindBestNonDeterministicChildToExpand(std::list<TreeNode*> const & children);
	TreeNode * FindBestChildToExpand(std::list<TreeNode*> const & children, std::list<double> const& total_simulations_ln_divides_child_count);
	TreeNode *FindBestChildToExpand(std::list<TreeNode*> const& children);
	double CalculateSelectionWeight(TreeNode *node, double total_simulations_ln_divides_node_simulations);

	bool ExpandNewNode(TreeNode * & node, GameEngine::Board & board);
	bool ExpandNodeWithDeterministicNextMoves(TreeNode * & node, GameEngine::Board & board);
	bool ExpandNodeWithSingleRandomNextMove(TreeNode * & node, GameEngine::Board & board);
	bool ExpandNodeWithMultipleRandomNextMoves(TreeNode * & node, GameEngine::Board & board);

private: // for internal use
	std::mt19937 random_generator;

	std::list<std::unique_ptr<BoardInitializer>> history_board_initializer; // a place to hold all history initialize boards
	std::unique_ptr<BoardInitializer> board_initializer;
	TreeNode * board_initializer_node;

	Tree tree;
	BoardFinder board_finder;

	TreeNode *allocated_node;
	std::list<TreeNode*> traversed_nodes;

	int current_start_board_random;
	TraversedPathRecorder traversed_path;

#ifdef DEBUG_PRINT_MOVE
	int iteration_count;
	std::ofstream debug_print_move;
#endif
};

#endif
