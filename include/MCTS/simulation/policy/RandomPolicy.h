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
				static int GetChoice(PolicyBase::ChoiceGetter const& choice_getter, Board const& board)
				{
					// TODO: use value network to enhance simulation
					//return std::rand() % choices;
					std::vector<int> valid_choices;
					choice_getter.ForEachChoice([&](int choice) {
						valid_choices.push_back(choice);
						return true;
					});
					int rnd = rand() % valid_choices.size();
					return valid_choices[rnd];
				}
			};
		}
	}
}