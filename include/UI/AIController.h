#pragma once

#include <chrono>

#include "state/State.h"
#include "MCTS/MOMCTS.h"

namespace ui
{
	class AIController
	{
	private:
		typedef state::State StartingStateGetter();

	public:
		AIController() :
			first_tree_(), second_tree_(), statistic_(),
			mcts_(first_tree_, second_tree_, statistic_)
		{}

		void Run(int duration_sec, StartingStateGetter* state_getter) {
			std::chrono::steady_clock::time_point end = 
				std::chrono::steady_clock::now() +
				std::chrono::seconds(duration_sec);

			long long last_show_rest_sec = -1;

			while (true) {
				auto now = std::chrono::steady_clock::now();
				if (now > end) break;
				
				auto rest_sec = std::chrono::duration_cast<std::chrono::seconds>(end - now).count();
				if (rest_sec != last_show_rest_sec) {
					std::cout << "Rest seconds: " << rest_sec << std::endl;
					last_show_rest_sec = rest_sec;
				}

				bool ret = mcts_.Iterate([&]() {
					return (*state_getter)();
				});

				if (ret) statistic_.IterateSucceeded();
				else statistic_.IterateFailed();
			}
		}

		auto const& GetStatistic() const { return statistic_; }

		auto GetRootNode(state::PlayerIdentifier side) const {
			return mcts_.GetRootNode(side);
		}

	private:
		mcts::builder::TreeBuilder::TreeNode first_tree_;
		mcts::builder::TreeBuilder::TreeNode second_tree_;
		mcts::Statistic<> statistic_;
		mcts::MOMCTS mcts_;
	};
}