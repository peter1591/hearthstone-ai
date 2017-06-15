#pragma once

#include "MCTS/board/Board.h"
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

					size_t Size() const { return tree_.GetWhiteListCount(); }

					int Get(size_t idx) const { return tree_.GetWhiteListItem(idx); }

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