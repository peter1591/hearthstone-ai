#pragma once

#include "MCTS/simulation/ChoiceBlacklist.h"
#include "MCTS/board/Board.h"

namespace mcts
{
	namespace policy
	{
		namespace simulation
		{
			using TreeProgress = mcts::simulation::ChoiceBlacklist;

			class ChoiceGetter
			{
			public:
				ChoiceGetter(int choices, TreeProgress const& progress) :
					choices_(choices), progress_(progress) {}

				size_t Size() const { return progress_.GetWhiteListCount(choices_); }

				int Get(size_t idx) const { return progress_.GetWhiteListItem(idx); }

				template <typename Functor>
				void ForEachChoice(Functor&& functor) const {
					progress_.ForEachWhiteListItem(choices_, std::forward<Functor>(functor));
				}

			private:
				int choices_;
				TreeProgress const& progress_;
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
					assert([&]() {
						int result2 = choice_getter.Get(rand_idx);
						return result == result2;
					}());
					return result;
				}
			};
		}
	}
}