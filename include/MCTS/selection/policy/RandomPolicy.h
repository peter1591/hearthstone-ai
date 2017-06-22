#pragma once

#include <vector>
#include "MCTS/board/Board.h"
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
					GetChoice(PolicyBase::ChoiceGetter const& choice_getter, board::Board const& board)
				{
					size_t count = choice_getter.Size();
					size_t idx = 0;
					size_t rand_idx = (size_t)(std::rand() % count);
					std::pair<int, TreeNode*> result;
					choice_getter.ForEach([&](int action, TreeNode* child) mutable {
						if (idx == rand_idx) {
							result = std::make_pair(action, child);
							return false;
						}
						++idx;
						return true;
					});
					std::pair<int, TreeNode*> result2 = choice_getter.Get(rand_idx);
					assert(result == result2);
					return result;
				}
			};
		}
	}
}