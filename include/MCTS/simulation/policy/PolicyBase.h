#pragma once

#include "MCTS/Board.h"
#include "MCTS/simulation/Tree.h"

namespace mcts
{
	namespace simulation
	{
		namespace policy
		{
			class PolicyBase
			{
			public:
				class ChoiceGetter
				{
				public:
					ChoiceGetter(Tree const& tree) : tree_(tree) {}

					template <typename Functor>
					void ForEachChoice(Functor&& functor) const {
						tree_.ForEachWhiteListItem(std::forward<Functor>(functor));
					}

				private:
					Tree const& tree_;
				};
			};
		}
	}
}