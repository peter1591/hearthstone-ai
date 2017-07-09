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
					struct Item {
						int choice;
						int chosen_times;
						TreeNode* node;
					};
					constexpr size_t kMaxChoices = 10; // max #-of-choices: choose hand card
					std::array<Item, kMaxChoices> choices;
					size_t choices_size = 0;

					// Phase 1: get total chosen times, and record to 'choices'
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

						TreeNode* node = choice_iterator.GetNode();
						assert(node);

						if (node->GetAddon().statistic.total <= 0) {
							// the leading edge is already chosen (i.e., chosen_times > 0)
							// so, this node should at least been traversed once
							assert(false);
						}

						assert(chosen_times > 0); // == 0
						total_chosen_times += chosen_times;

						assert(choices_size < kMaxChoices);
						choices[choices_size] = Item{ choice, chosen_times, node };
						++choices_size;
					}

					if (total_chosen_times == 0) {
						return -1; // no valid choice
					}

					// Phase 2: use UCB to make a choice
					auto get_score = [total_chosen_times](Item const& item) {
						int wins = item.node->GetAddon().statistic.credit;
						int total = item.node->GetAddon().statistic.total;
						assert(total > 0);
						double exploit_score = ((double)wins) / total;

						constexpr double explore_weight = 0.8;
						double explore_score = ((double)item.chosen_times) / total_chosen_times;

						return exploit_score + explore_weight * explore_score;
					};

					assert(choices_size > 0);
					size_t idx = 0;

					size_t best_choice = idx;
					double best_score = get_score(choices[idx]);
					++idx;

					for (; idx < choices_size; ++idx) {
						double score = get_score(choices[idx]);
						if (score > best_score) {
							best_choice = idx;
							best_score = score;
						}
					}

					return (int)choices[best_choice].choice;
				}
			};
		}
	}
}