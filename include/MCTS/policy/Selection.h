#pragma once

#include "MCTS/selection/TreeNode.h"
#include "MCTS/board/Board.h"
#include "MCTS/selection/EdgeAddon.h"

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
				void AddChoice(int choice, mcts::selection::EdgeAddon const& addon, TreeNode* node) {
					if (!node) return;
					assert(addon.chosen_times > 0);
					choices_.push_back(Item{ choice, addon.chosen_times, node });
				}
				int SelectChoice() {
					if (choices_.empty()) return -1;
					int target = rand() % choices_.size();
					return choices_[target].choice;
				}

			private:
				struct Item {
					int choice;
					int chosen_times;
					TreeNode* node;
				};
				std::vector<Item> choices_;
			};
		};
	}
}