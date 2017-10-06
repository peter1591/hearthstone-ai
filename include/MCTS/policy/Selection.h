#pragma once

#include <cmath>

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
				RandomPolicy(std::mt19937 & rand) : rand_(rand) {}

				void ReportChoicesCount(int count) {
					// the count here contains the invalid actions
					// cannot use directly
				}
				void AddChoice(int choice, mcts::selection::EdgeAddon const& addon, TreeNode* node) {
					assert(node);
					assert(addon.GetChosenTimes() > 0);
					choices_.push_back(Item{ choice, addon.GetChosenTimes(), node });
				}
				int SelectChoice() {
					if (choices_.empty()) return -1;
					int target = rand_() % choices_.size();
					return choices_[target].choice;
				}

			private:
				struct Item {
					int choice;
					std::int64_t chosen_times;
					TreeNode* node;
				};
				std::vector<Item> choices_;
				std::mt19937 & rand_;
			};

			class UCBPolicy {
			public:
				static constexpr double kExploreWeight = 0.8;

				UCBPolicy(state::PlayerSide side) : side_(side) {}

				template <typename ChoiceIterator>
				int SelectChoice(ChoiceIterator && choice_iterator)
				{
					struct Item {
						int choice;
						mcts::selection::EdgeAddon const* edge_addon;
					};
					constexpr size_t kMaxChoices = FlowControl::IActionParameterGetter::kMaxChoices;
					std::array<Item, kMaxChoices> choices;
					size_t choices_size = 0;

					// Phase 1: get total chosen times, and record to 'choices'
					std::int64_t total_chosen_times = 0;
					for (choice_iterator.Begin();
						!choice_iterator.IsEnd();
						choice_iterator.StepNext())
					{
						typename ChoiceIterator::CheckResult check_result = choice_iterator.Check();

						int choice = choice_iterator.GetChoice();
						if (check_result == ChoiceIterator::CheckResult::kForceSelectChoice) {
							return choice;
						}

						auto const& edge_addon = choice_iterator.GetAddon();
						auto chosen_times = edge_addon.GetChosenTimes();
						if (chosen_times == 0) {
							return choice; // force select
						}
						if (edge_addon.GetTotal() == 0) {
							// a node is created (from another thread),
							// but is not yet updated from that thread
							// in this case, we just force select that choice
							return choice;
						}

						assert(chosen_times > 0); // == 0
						total_chosen_times += chosen_times;

						assert(choices_size < kMaxChoices);
						choices[choices_size] = Item{ choice, &edge_addon };
						++choices_size;
					}

					assert(total_chosen_times > 0);

					// Phase 2: use UCB to make a choice
					auto get_score = [total_chosen_times](Item const& item) {
						auto total = item.edge_addon->GetTotal();
						auto wins = item.edge_addon->GetCredit();
						assert(total > 0);
						assert(wins <= total);
						double exploit_score = ((double)wins) / total;

						auto chosen_times = item.edge_addon->GetChosenTimes();
						// in case another thread visited it
						if (chosen_times > total_chosen_times) chosen_times = total_chosen_times;
						double explore_score = std::sqrt(
							std::log((double)total_chosen_times) / chosen_times);

						return exploit_score + kExploreWeight * explore_score;
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

			private:
				state::PlayerSide side_;
			};
		}
	}
}