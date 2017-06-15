#pragma once

#include "state/IRandomGenerator.h"
#include "MCTS/MCTS.h"

namespace mcts
{
	namespace board
	{
		class RandomGenerator : public state::IRandomGenerator
		{
		public:
			RandomGenerator(MCTS & mcts) : mcts_(mcts) {}

			int Get(int exclusive_max) final
			{
				return mcts_.ChooseAction(ActionType(ActionType::kRandom), exclusive_max);
			}

			size_t Get(size_t exclusive_max) final
			{
				return (size_t)mcts_.ChooseAction(ActionType(ActionType::kRandom), (int)exclusive_max);
			}

			// @param min Inclusive minimum
			// @param max Inclusive maximum
			int Get(int min, int max) final
			{
				assert(max >= min);
				return min + Get(max - min + 1);
			}

		private:
			MCTS & mcts_;
		};
	}
}