#include "decider.h"
#include "mcts-merger.h"

static void PrintLevelPrefix(int level)
{
	for (int i = 0; i<level; i++) {
		std::cout << "..";
	}
}

static TreeNode *FindBestChildToPlay(TreeNode *node)
{
	if (node->children.empty()) throw std::runtime_error("failed");

	TreeNode *most_simulated = nullptr;
	for (const auto &child : node->children) {
		if (most_simulated == nullptr ||
			child->count > most_simulated->count)
		{
			most_simulated = child;
		}
	}

	return most_simulated;
}

void Decider::PrintBestRoute(int levels)
{
	TreeNode *node = &this->mcts.tree.GetRootNode();

	int level = 0;
	while (!node->children.empty() && level <= levels) {

		if (node->move.action != GameEngine::Move::ACTION_GAME_FLOW) {
			PrintLevelPrefix(level);
			std::cout << "[" << node->stage << "] ";
			if (node != &this->mcts.tree.GetRootNode()) {
				std::cout << node->move.GetDebugString();
			}
			std::cout << " " << node->wins << "/" << node->count << std::endl;
			level++;
		}

		node = FindBestChildToPlay(node);
	}
}

Decider::Decider(MCTS &&mcts) : mcts(std::move(mcts))
{
}

void Decider::DebugPrint()
{
	this->PrintBestRoute(20);
}

void Decider::Merge(MCTS&& mcts)
{
	MCTSMerger merger(this->mcts);
	merger.Merge(std::move(mcts));
}