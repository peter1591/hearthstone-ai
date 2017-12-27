#pragma once

#include <functional>
#include <random>

#include <state/State.h>
#include "MCTS/MOMCTS.h"
#include "judge/Judger.h"

namespace agents
{
	class MCTSRunner
	{
	public:
		MCTSRunner(int tree_samples, std::mt19937 & rand) :
			threads_(), rand_(rand),
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

		template <class StateGetter>
		void Run(int thread_count, StateGetter && state_getter)
		{
			assert(threads_.empty());
			stop_flag_ = false;
			for (int i = 0; i < thread_count; ++i) {
				int thread_seed = rand_();
				threads_.emplace_back([this, thread_seed, state_getter]() {
					std::mt19937 selection_rand;
					std::mt19937 simulation_rand(thread_seed);
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
							return state_getter(selection_rand);
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
		std::mt19937 & rand_;
		mcts::builder::TreeBuilder::TreeNode first_tree_;
		mcts::builder::TreeBuilder::TreeNode second_tree_;
		mcts::Statistic<> statistic_;
		std::atomic_bool stop_flag_;
		std::vector<int> tree_sample_randoms_;
	};
}