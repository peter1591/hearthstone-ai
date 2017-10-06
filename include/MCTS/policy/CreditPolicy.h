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
				// we want the game ends as early as possible,
				// so we have a higher score if the game ends at an eariler turn
				//int turn = game_state.GetTurn();
				//static constexpr int kMaxEffectiveTurn = 20;
				//turn = std::min(turn, kMaxEffectiveTurn);
				//static constexpr int kStartTurn = 5;
				//turn -= kStartTurn;
				//turn = std::max(turn, 0);
				//double score = (double)(kMaxEffectiveTurn - turn) / kMaxEffectiveTurn;

				double score = result.score_;

				static constexpr double kMinScore = 0.5;
				score = (score * (1.0 - kMinScore)) + kMinScore;
				
				assert(result.type_ == Result::kResultWin || result.type_ == Result::kResultLoss);
				double sign = 1.0;
				if (side == state::kPlayerFirst) {
					if (result.type_ == Result::kResultWin) sign = 1.0;
					else sign = -1.0;
				}
				else {
					assert(side == state::kPlayerSecond);
					if (result.type_ == Result::kResultLoss) sign = 1.0;
					else sign = -1.0;
				}
				score = sign * score;

				// scale from [-1, 1] to [0, 1]
				score = (score + 1.0) * 0.5;
				return score;
			}
		};
	}
}