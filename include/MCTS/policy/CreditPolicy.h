#pragma once

#include "MCTS/Types.h"

namespace mcts
{
	namespace policy
	{
		class CreditPolicy
		{
		public:
			static double GetCredit(state::State const& game_state, state::PlayerSide side, Result result) {
				double score = result.score_;

				static constexpr double kMinScore = 0.5;
				score = (score * (1.0 - kMinScore)) + kMinScore;
				
				assert(result.type_ == Result::kResultWin || result.type_ == Result::kResultLoss);
				if (side == state::kPlayerSecond) {
					score = -score;
				}

				// scale from [-1, 1] to [0, 1]
				score = (score + 1.0) * 0.5;
				return score;
			}
		};
	}
}