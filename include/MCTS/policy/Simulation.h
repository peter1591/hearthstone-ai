#pragma once

#include "MCTS/simulation/Tree.h"
#include "MCTS/board/Board.h"

namespace mcts
{
	namespace policy
	{
		namespace simulation
		{
			using Tree = mcts::simulation::Tree;

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

			class RandomPolicy
			{
			public:
				static int GetChoice(ChoiceGetter const& choice_getter, board::Board const& board)
				{
					// TODO: use value network to enhance simulation
					size_t count = choice_getter.Size();
					assert(count > 0);
					size_t idx = 0;
					size_t rand_idx = (size_t)(std::rand() % count);
					int result = -1;
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