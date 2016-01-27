#pragma once

#include "mcts.h"

class MCTSMerger
{
public:
	MCTSMerger(MCTS &mcts);

	void Merge(MCTS &&mcts);

	MCTS& Get();

private:
	void Merge(TreeNode *source, TreeNode *target, const GameEngine::Board &board, MCTS &&source_mcts);
	void MergeToTargetWithNoChildren(TreeNode *source, TreeNode *target, const GameEngine::Board &board, MCTS &&source_mcts);
	void MergeToParent(TreeNode *source, TreeNode *parent, const GameEngine::Board &parent_board,
		std::unordered_set<GameEngine::Move> &parent_rest_moves, MCTS&& source_mcts);

private:
	MCTS &mcts;
};