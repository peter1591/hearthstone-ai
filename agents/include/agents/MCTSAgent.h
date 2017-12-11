#pragma once

#include <atomic>
#include <chrono>
#include <thread>

#include "state/State.h"
#include "MCTS/MOMCTS.h"
#include "judge/IAgent.h"
#include "judge/Judger.h"

namespace agents
{
	class MCTSRunner
	{
	private:
		using StartingStateGetter = std::function<state::State(int)>;

	public:
		MCTSRunner(int tree_samples, std::mt19937 & rand) :
			threads_(),
			first_tree_(), second_tree_(), statistic_(), stop_flag_(false), tree_sample_randoms_()
		{
			for (int i = 0; i < tree_samples; ++i) {
				tree_sample_randoms_.push_back(rand());
			}
		}

		~MCTSRunner()
		{
			WaitUntilStopped();
		}

		void Run(int thread_count, int seed, StartingStateGetter state_getter)
		{
			assert(threads_.empty());
			stop_flag_ = false;
			for (int i = 0; i < thread_count; ++i) {
				threads_.emplace_back([this, seed, state_getter]() {
					std::mt19937 selection_rand;
					std::mt19937 simulation_rand(seed);
					mcts::MOMCTS mcts(first_tree_, second_tree_, statistic_, selection_rand, simulation_rand);

					size_t tree_sample_random_idx = 0;
					auto get_next_selection_seed = [tree_sample_random_idx, this]() mutable {
						int v = tree_sample_randoms_[tree_sample_random_idx];
						++tree_sample_random_idx;
						if (tree_sample_random_idx >= tree_sample_randoms_.size()) {
							tree_sample_random_idx = 0;
						}
						return v;
					};

					while (true) {
						if (stop_flag_ == true) break; // TODO: use compare_exchange_weak

						int sample_seed = get_next_selection_seed();
						selection_rand.seed(sample_seed);
						mcts.Iterate([&]() {
							return state_getter(sample_seed);
						});

						statistic_.IterateSucceeded();
					}
				});
			}
		}

		int NotifyStop() {
			stop_flag_ = true;
			return 0;
		}

		bool IsStopping() const {
			return stop_flag_;
		}

		void WaitUntilStopped()
		{
			NotifyStop();
			for (auto & thread : threads_) {
				thread.join();
			}
			threads_.clear();
		}

		auto const& GetStatistic() const { return statistic_; }

		auto GetRootNode(state::PlayerIdentifier side) const {
			if (side == state::kPlayerFirst) return &first_tree_;
			assert(side == state::kPlayerSecond);
			return &second_tree_;
		}

	private:
		std::vector<std::thread> threads_;
		mcts::builder::TreeBuilder::TreeNode first_tree_;
		mcts::builder::TreeBuilder::TreeNode second_tree_;
		mcts::Statistic<> statistic_;
		std::atomic_bool stop_flag_;
		std::vector<int> tree_sample_randoms_;
	};

	class MCTSAgent : public judge::IAgent {
	public:
		MCTSAgent() : root_node_(nullptr), node_(nullptr), controller_() {}

		MCTSAgent(MCTSAgent const&) = delete;
		MCTSAgent & operator=(MCTSAgent const&) = delete;

		void Think(state::State const& state, int threads, int seed, int tree_samples, std::function<bool(uint64_t)> cb) {
			auto continue_checker = [&]() {
				uint64_t iterations = controller_->GetStatistic().GetSuccededIterates();
				return cb(iterations);
			};

			std::mt19937 rand(seed);
			controller_.reset(new MCTSRunner(tree_samples, rand));
			controller_->Run(threads, rand(), [&](int seed) {
				(void)seed;
				return state;
			});

			while (true) {
				if (!continue_checker()) break;
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
			controller_->WaitUntilStopped();

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
		std::unique_ptr<MCTSRunner> controller_;
	};
}