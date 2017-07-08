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
				template <typename ChoiceIterator>
				int SelectChoice(ChoiceIterator && choice_iterator)
				{
					// Phase 1: get total chosen times
					int total_chosen_times = 0;
					for (choice_iterator.Begin();
						!choice_iterator.IsEnd();
						choice_iterator.StepNext())
					{
						typename ChoiceIterator::CheckResult check_result = choice_iterator.Check();
						if (check_result == ChoiceIterator::CheckResult::kInvalidChoice) {
							continue;
						}

						int choice = choice_iterator.GetChoice();
						if (check_result == ChoiceIterator::CheckResult::kForceSelectChoice) {
							return choice;
						}

						int chosen_times = choice_iterator.GetAddon().chosen_times;
						if (chosen_times == 0) return choice; // force select

						if (choice_iterator.GetNode()->GetAddon().statistic.total <= 0) {
							// shoult not happen?
							// assert(false); // TODO
						}

						assert(chosen_times > 0); // == 0
						total_chosen_times += chosen_times;
					}

					if (total_chosen_times == 0) {
						return -1; // no valid choice
					}

					// Phase 2: use UCB to make a choice
					int best_choice = -1;
					double best_score = 0.0;
					auto get_score = [total_chosen_times](int choice, mcts::selection::EdgeAddon const& addon, TreeNode* node) {
						int wins = node->GetAddon().statistic.credit;
						int total = node->GetAddon().statistic.total;
						double exploit_score = ((double)wins) / total;

						constexpr double explore_weight = 0.8;
						double explore_score = ((double)addon.chosen_times) / total_chosen_times;

						return exploit_score + explore_weight * explore_score;
					};

					for (choice_iterator.Begin();
						!choice_iterator.IsEnd();
						choice_iterator.StepNext())
					{
						typename ChoiceIterator::CheckResult check_result = choice_iterator.Check();
						if (check_result == ChoiceIterator::CheckResult::kInvalidChoice) {
							continue;
						}

						int choice = choice_iterator.GetChoice();
						if (check_result == ChoiceIterator::CheckResult::kForceSelectChoice) {
							assert(false); // should be taken care in the previous loop
							return choice;
						}

						assert(check_result == ChoiceIterator::CheckResult::kNormalChoice);
						double score = get_score(
							choice,
							choice_iterator.GetAddon(),
							choice_iterator.GetNode());
						if (best_choice < 0 || score > best_score) {
							best_choice = choice;
							best_score = score;
						}
					}

					assert(best_choice >= 0);
					return best_choice;
				}
			};
		}
	}
}