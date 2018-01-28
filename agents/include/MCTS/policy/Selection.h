#pragma once

#include <cmath>
#include <random>

#include "MCTS/selection/TreeNode.h"
#include "MCTS/selection/EdgeAddon.h"
#include "engine/view/Board.h"

namespace mcts
{
	namespace policy
	{
		namespace selection
		{
			using TreeNode = mcts::selection::TreeNode;

			class ChoiceIterator {
			public:
				struct Item {
					int choice;
					mcts::selection::EdgeAddon const* edge_addon;
				};

				ChoiceIterator(engine::ActionChoices & choices, mcts::selection::ChildNodeMap const& children) :
					choices_(choices), children_(children)
				{}

				void Begin() { choices_.Begin(); }
				void StepNext() { choices_.StepNext(); }
				bool IsEnd() { return choices_.IsEnd(); }

				void Get(Item & item) {
					item.choice = choices_.Get();
					assert(item.choice >= 0);
					item.edge_addon = children_.Get(item.choice).first;
				}

			private:
				engine::ActionChoices & choices_;
				mcts::selection::ChildNodeMap const& children_;
			};

			class RandomPolicy {
			public:
				RandomPolicy(std::mt19937 & rand) : choices_(), rand_(rand) {}

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
				static constexpr double kExploreWeight = 0.2;

				int SelectChoice(engine::ActionType action_type, ChoiceIterator choice_iterator)
				{
					constexpr size_t kMaxChoices = engine::IActionParameterGetter::kMaxChoices;
					std::array<ChoiceIterator::Item, kMaxChoices> choices;
					size_t choices_size = 0;

					// Phase 1: get total chosen times, and record to 'choices'
					std::int64_t total_chosen_times = 0;
					for (choice_iterator.Begin(); !choice_iterator.IsEnd(); choice_iterator.StepNext())
					{
						choice_iterator.Get(choices[choices_size]);
						auto & item = choices[choices_size];
						int choice = item.choice;
						auto edge_addon = item.edge_addon;

						if (!edge_addon) return choice; // force select

						auto chosen_times = edge_addon->GetChosenTimes();
						if (chosen_times == 0) {
							return choice; // force select
						}
						if (edge_addon->GetTotal() == 0) {
							// a node is created (from another thread),
							// but is not yet updated from that thread
							// in this case, we just force select that choice
							return choice;
						}

						assert(chosen_times > 0);
						total_chosen_times += chosen_times;

						assert(choices_size < kMaxChoices);
						++choices_size;
					}

					assert(total_chosen_times > 0);

					// Phase 2: use UCB to make a choice
					auto get_score = [total_chosen_times](
						engine::ActionType action_type,
						size_t choice_idx, size_t choice_count,
						ChoiceIterator::Item const& item)
					{
						double explore_weight = kExploreWeight;
						if (action_type == engine::ActionType::kMainAction) {
							if (choice_idx == choice_count - 1) {
								// do not choose end-turn action
								// this is a simple mimic to policy network as in AlphaZero
								explore_weight *= 0.1;
							}
						}

						double exploit_score = item.edge_addon->GetAverageCredit();
						assert(exploit_score >= -1.0);
						assert(exploit_score <= 1.0);

						auto chosen_times = item.edge_addon->GetChosenTimes();
						// in case another thread visited it
						if (chosen_times > total_chosen_times) chosen_times = total_chosen_times;
						double explore_score = std::sqrt(
							std::log((double)total_chosen_times) / chosen_times);

						return exploit_score + explore_weight * explore_score;
					};

					assert(choices_size > 0);

					size_t best_choice = 0;
					double best_score = -std::numeric_limits<double>::infinity();

					for (size_t idx = 0; idx < choices_size; ++idx) {
						double score = get_score(action_type, idx, choices_size, choices[idx]);
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