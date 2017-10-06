#pragma once

#include "MCTS/Types.h"

namespace mcts
{
	namespace policy
	{
		class CreditPolicy
		{
		public:
			// Return the credit value to update the tree nodes
			// * If the first player has 100% to win, the credit should be 1.0
			// * If the first player has 100% to loss, the credit should be 0.0
			// * If the first player has 50% to win, the credit should be 0.5
			// * The credit value should be in range [0.0, 1.0]
			static double GetCredit(state::State const& game_state, Result result) {
				double score = result.score_;

				static constexpr double kMinScore = 0.5;
				score = (score * (1.0 - kMinScore)) + kMinScore;
				
				assert(result.type_ == Result::kResultWin || result.type_ == Result::kResultLoss);

				// scale from [-1, 1] to [0, 1]
				score = (score + 1.0) * 0.5;
				return score;
			}
		};
	}
}