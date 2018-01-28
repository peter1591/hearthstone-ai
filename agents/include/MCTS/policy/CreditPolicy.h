#pragma once

#include "engine/view/Board.h"
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
			// * If the first player has 100% to loss, the credit should be -1.0
			// * If the first player has 50% to win, the credit should be 0.0
			// * The credit value should be in range [-1.0, 1.0]
			static float GetCredit(engine::view::Board const& board, StateValue state_value) {
				auto side = board.GetViewSide();
				return state_value.GetValue(side);
			}
		};
	}
}