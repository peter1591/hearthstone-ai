#pragma once

#include "MCTS/selection/TreeNode.h"
#include "MCTS/board/Board.h"

namespace mcts
{
	namespace policy
	{
		namespace selection
		{
			using TreeNode = mcts::selection::TreeNode;

			class RandomPolicy {
			public:
				void ReportChoicesCount(int count) {
					// the count here contains the invalid actions
					// cannot use directly
				}
				void AddChoice(int choice, TreeNode* node) {
					if (!node) return;
					choices_.push_back({ choice, node });
				}
				std::pair<int, TreeNode*> SelectChoice() {
					if (choices_.empty()) return { -1, nullptr };
					int target = rand() % choices_.size();
					return choices_[target];
				}

			private:
				typedef std::pair<int, TreeNode*> Result;
				std::vector<Result> choices_;
			};
		};
	}
}