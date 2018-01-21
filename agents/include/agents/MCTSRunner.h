#pragma once

#include <functional>
#include <random>

#include "engine/view/BoardView.h"
#include "engine/view/board_view/StateRestorer.h"
#include "state/State.h"
#include "MCTS/MOMCTS.h"
#include "judge/Judger.h"
#include "agents/MCTSConfig.h"

namespace agents
{
	class MCTSRunner
	{
	public:
		MCTSRunner(MCTSAgentConfig const& config, std::mt19937 & rand) :
			config_(config), rand_(rand),
			first_tree_(), second_tree_(), statistic_(), stop_flag_(false), tree_sample_randoms_()
		{
			for (int i = 0; i < config_.tree_samples; ++i) {
				tree_sample_randoms_.push_back(rand());
			}
		}

		~MCTSRunner()
		{
			WaitUntilStopped();
		}

		void Run(engine::view::BoardRefView const& game_state)
		{
			assert(threads_.empty());
			stop_flag_ = false;
			for (int i = 0; i < config_.threads; ++i) {
				int thread_seed = rand_();
				threads_.emplace_back([this, thread_seed, game_state]() {
					engine::view::BoardView board_view;
					engine::view::board_view::UnknownCardsInfo first_unknown;
					engine::view::board_view::UnknownCardsInfo second_unknown;

					// TODO: guess/feed deck type
					// TODO: remove revealed/played/removed cards
					first_unknown.deck_cards_ = decks::Decks::GetDeckCards("InnKeeperExpertWarlock");
					second_unknown.deck_cards_ = decks::Decks::GetDeckCards("InnKeeperExpertWarlock");

					board_view.Parse(game_state, first_unknown, second_unknown);
					auto state_restorer = engine::view::board_view::StateRestorer::Prepare(
						board_view, first_unknown, second_unknown);
					auto state_getter = [&](std::mt19937 & rnd) -> state::State {
						return state_restorer.RestoreState(rnd);
					};


					std::mt19937 selection_rand;
					std::mt19937 simulation_rand(thread_seed);
					mcts::MOMCTS mcts(first_tree_, second_tree_, statistic_, selection_rand, simulation_rand, config_.mcts);

					size_t tree_sample_random_idx = 0;
					auto get_next_selection_seed = [tree_sample_random_idx, this]() mutable {
						int v = tree_sample_randoms_[tree_sample_random_idx];
						++tree_sample_random_idx;
						if (tree_sample_random_idx >= tree_sample_randoms_.size()) {
							tree_sample_random_idx = 0;
						}
						return v;
					};

					while (!stop_flag_.load()) {
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
		MCTSAgentConfig config_;
		std::vector<std::thread> threads_;
		std::mt19937 & rand_;
		mcts::selection::TreeNode first_tree_;
		mcts::selection::TreeNode second_tree_;
		mcts::Statistic<> statistic_;
		std::atomic_bool stop_flag_;
		std::vector<int> tree_sample_randoms_;
	};
}