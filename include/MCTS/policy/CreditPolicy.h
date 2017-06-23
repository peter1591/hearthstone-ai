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
				assert(result != Result::kResultInvalid);
				assert(result != Result::kResultNotDetermined);
				if (side == state::kPlayerFirst) {
					// only get credit for wins; no credit for draws
					if (result == Result::kResultFirstPlayerWin) return true;
					else return false;
				}
				else {
					assert(side == state::kPlayerSecond);
					return !GetCredit(state::kPlayerFirst, result);
				}
			}
		};
	}
}