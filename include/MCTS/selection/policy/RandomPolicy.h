#pragma once

#include <vector>
#include "MCTS/Board.h"
#include "MCTS/selection/policy/PolicyBase.h"

namespace mcts
{
	namespace selection
	{
		namespace policy
		{
			class RandomPolicy
			{
			public:
				static std::pair<int, TreeNode*>
					GetChoice(PolicyBase::ChoiceGetter const& choice_getter, Board const& board)
				{
					std::vector<std::pair<int, TreeNode*>> valid_choices;
					choice_getter.ForEachChoice([&](int action, TreeNode* child) {
						valid_choices.push_back({ action, child });
					});
					int idx = std::rand() % valid_choices.size();
					return valid_choices[idx];
				}
			};
		}
	}
}