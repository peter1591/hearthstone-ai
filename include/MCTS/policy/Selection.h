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
					assert(node);
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

			class UCBPolicy {
			public:
				void ReportChoicesCount(int count) {
					// the count here contains the invalid actions
					// cannot use directly
				}
				void AddChoice(int choice, mcts::selection::EdgeAddon const& addon, TreeNode* node) {
					assert(node);
					assert(addon.chosen_times > 0);
					choices_.push_back(Item{ choice, addon.chosen_times, node});
					total_chosen_times += addon.chosen_times;
				}
				int SelectChoice() {
					// TODO: pass a iterable object here, so we don't need a container to store the items
					if (choices_.empty()) return -1;

					auto get_score = [this](Item const& item) {
						int wins = item.node->GetAddon().statistic.credit;
						int total = item.node->GetAddon().statistic.total;
						double exploit_score = ((double)wins) / total;

						constexpr double explore_weight = 0.8;
						double explore_score = ((double)item.chosen_times) / total_chosen_times;

						return exploit_score + explore_weight * explore_score;
					};

					int max_choice = -1;
					double max_score = 0.0;

					for (auto const& item : choices_) {
						double score = get_score(item);
						if (score > max_score) {
							max_score = score;
							max_choice = item.choice;
						}
					}
					assert(max_choice >= 0);
					return max_choice;
				}

			private:
				struct Item {
					int choice;
					int chosen_times;
					TreeNode* node;
				};
				std::vector<Item> choices_;
				int total_chosen_times;
			};
		};
	}
}