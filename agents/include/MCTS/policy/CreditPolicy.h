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

			static double GetCredit(engine::view::Board const& board, engine::Result result) {
				auto side = board.GetViewSide();
				bool winning = WinOrLoss(side, result);

				double score = 0.0;
				if (winning) score = 1.0;
				else score = -1.0;

				static constexpr double kMinScore = 0.5;
				score = (score * (1.0 - kMinScore)) + kMinScore;
				
				// scale from [-1, 1] to [0, 1]
				score = (score + 1.0) * 0.5;
				return score;
			}

		private:
			static bool WinOrLoss(state::PlayerSide side, engine::Result result) {
				if (side == state::kPlayerFirst) {
					switch (result) {
					case engine::kResultFirstPlayerWin:
						return true;
					case engine::kResultSecondPlayerWin:
					case engine::kResultDraw:
						return false;
					default:
						assert(false);
						throw std::runtime_error("logic error");
					}
				}
				else if (side == state::kPlayerSecond) {
					switch (result) {
					case engine::kResultSecondPlayerWin:
						return true;
					case engine::kResultFirstPlayerWin:
					case engine::kResultDraw:
						return false;
					default:
						assert(false);
						throw std::runtime_error("logic error");
					}
				}
				else {
					assert(false);
					throw std::runtime_error("logic error");
				}
			}
		};
	}
}