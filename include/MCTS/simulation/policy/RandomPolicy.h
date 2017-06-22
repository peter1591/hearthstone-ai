#pragma once

#include "MCTS/simulation/policy/PolicyBase.h"

namespace mcts
{
	namespace simulation
	{
		namespace policy
		{
			class RandomPolicy
			{
			public:
				static int GetChoice(PolicyBase::ChoiceGetter const& choice_getter, board::BoardOnlineViewer const& board)
				{
					// TODO: use value network to enhance simulation
					//return std::rand() % choices;
					size_t count = choice_getter.Size();
					size_t idx = 0;
					size_t rand_idx = (size_t)(rand() % count);
					int result;
					choice_getter.ForEachChoice([&](int choice) {
						if (idx == rand_idx) {
							result = choice;
							return false;
						}
						++idx;
						return true;
					});
					int result2 = choice_getter.Get(rand_idx);
					assert(result == result2);
					return result;
				}
			};
		}
	}
}