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
			statistic_(),
			mcts_(statistic_)
		{}

		void Run(int duration_sec, StartingStateGetter* state_getter) {
			std::chrono::steady_clock::time_point end = 
				std::chrono::steady_clock::now() +
				std::chrono::seconds(duration_sec);

			while (std::chrono::steady_clock::now() < end) {
				bool ret = mcts_.Iterate([&]() {
					return (*state_getter)();
				});

				if (ret) statistic_.IterateSucceeded();
				else statistic_.IterateFailed();
			}
		}

		auto const& GetStatistic() const { return statistic_; }

	private:
		mcts::Statistic<> statistic_;
		mcts::MOMCTS mcts_;
	};
}