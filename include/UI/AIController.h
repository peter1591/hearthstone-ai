#pragma once

#include <atomic>
#include <chrono>
#include <thread>

#include "state/State.h"
#include "MCTS/MOMCTS.h"
#include "UI/CompetitionGuide.h"

namespace ui
{
	class AIController
	{
	private:
		using StartingStateGetter = std::function<state::State(int)>;

	public:
		AIController() :
			first_tree_(), second_tree_(), statistic_(), stop_flag_(false)
		{}

		template <class ContinueChecker>
		void Run(ContinueChecker continue_checker, int thread_count, int seed, StartingStateGetter state_getter)
		{
			std::vector<std::thread> threads;
			stop_flag_ = false;
			for (int i = 0; i < thread_count; ++i) {
				threads.emplace_back([&]() {
					std::mt19937 rand(seed);
					mcts::MOMCTS mcts(first_tree_, second_tree_, statistic_, rand);
					while (true) {
						if (stop_flag_ == true) break; // TODO: use compare_exchange_weak

						mcts.Iterate([&]() {
							return state_getter(rand());
						});

						statistic_.IterateSucceeded();
					}
				});
			}

			while (true) {
				if (stop_flag_ == true) break; // TODO: use compare_exchange_weak
				if (!continue_checker()) break;
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}

			stop_flag_ = true;
			for (auto & thread : threads) {
				thread.join();
			}
		}

		int NotifyStop() {
			stop_flag_ = true;
			return 0;
		}

		auto const& GetStatistic() const { return statistic_; }

		auto GetRootNode(state::PlayerIdentifier side) const {
			if (side == state::kPlayerFirst) return &first_tree_;
			assert(side == state::kPlayerSecond);
			return &second_tree_;
		}

	private:
		mcts::builder::TreeBuilder::TreeNode first_tree_;
		mcts::builder::TreeBuilder::TreeNode second_tree_;
		mcts::Statistic<> statistic_;
		std::atomic_bool stop_flag_;
	};

	class AICompetitor : public ICompetitor {
	public:
		AICompetitor() : root_node_(nullptr), node_(nullptr), controller_() {}

		AICompetitor(AICompetitor const&) = delete;
		AICompetitor & operator=(AICompetitor const&) = delete;

		void Think(state::State const& state, int threads, int seed, std::function<bool(uint64_t)> cb) {
			auto continue_checker = [&]() {
				uint64_t iterations = controller_->GetStatistic().GetSuccededIterates();
				return cb(iterations);
			};

			controller_.reset(new AIController());
			controller_->Run(continue_checker, threads, seed, [&](int seed) {
				(void)seed;
				return state;
			});
			node_ = controller_->GetRootNode(state.GetCurrentPlayerId());
			root_node_ = node_;
		}

		int GetMainAction() final {
			assert(root_node_ == node_);
			assert(node_);
			assert(node_->GetActionType().GetType() == mcts::ActionType::kMainAction);

			int best_choice = -1;
			double best_chosen_times = -std::numeric_limits<double>::infinity();
			mcts::builder::TreeBuilder::TreeNode const* best_node = nullptr;
			root_node_->ForEachChild([&](int choice, mcts::selection::ChildType const& child) {
				double chosen_times = (double)child.GetEdgeAddon().GetChosenTimes();
				if (chosen_times > best_chosen_times) {
					best_chosen_times = chosen_times;
					best_choice = choice;
					best_node = child.GetNode();
				}
				return true;
			});

			node_ = best_node;

			return best_choice;
		}

		mcts::board::BoardActionAnalyzer GetActionApplier() final {
			return root_node_->GetAddon().action_analyzer;
		}

		int GetSubAction(mcts::ActionType::Types action_type, mcts::board::ActionChoices action_choices) final {
			assert(node_);

			if (node_->GetActionType() != action_type) {
				assert(false);
				throw std::runtime_error("Action type not match");
			}

			auto CanBeChosen = [&](int choice) {
				for (action_choices.Begin(); !action_choices.IsEnd(); action_choices.StepNext()) {
					if (action_choices.Get() == choice) return true;
				}
				return false;
			};

			int best_choice = -1;
			double best_chosen_times = -std::numeric_limits<double>::infinity();
			mcts::builder::TreeBuilder::TreeNode const* best_node = nullptr;
			node_->ForEachChild([&](int choice, mcts::selection::ChildType const& child) {
				if (!CanBeChosen(choice)) return true;

				double chosen_times = (double)child.GetEdgeAddon().GetChosenTimes();
				if (chosen_times > best_chosen_times) {
					best_chosen_times = chosen_times;
					best_choice = choice;
					best_node = child.GetNode();
				}
				return true;
			});

			if (best_choice < 0) {
				throw std::runtime_error("No any choice is evaluated.");
			}

			node_ = best_node;
			return best_choice;
		}

	private:
		mcts::builder::TreeBuilder::TreeNode const* root_node_;
		mcts::builder::TreeBuilder::TreeNode const* node_;
		std::unique_ptr<AIController> controller_;
	};
}