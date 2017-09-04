#pragma once

#include "MCTS/Types.h"

namespace mcts
{
	namespace policy
	{
		class CreditPolicy
		{
		public:
			static bool GetCredit(state::PlayerSide side, Result result) {
				assert(result == Result::kResultWin || result == Result::kResultLoss);
				if (side == state::kPlayerFirst) return (result == Result::kResultWin);
				else {
					assert(side == state::kPlayerSecond);
					return (result == Result::kResultLoss);
				}
			}
		};
	}
}