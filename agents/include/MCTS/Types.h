#pragma once

#include <assert.h>
#include "state/Types.h"

namespace mcts
{
	class StateValue
	{
	public:
		StateValue() : v_(0.0f) {}

		void SetValue(float value_for_current_player, state::PlayerSide current_player) {
			v_ = value_for_current_player;
			if (current_player != state::kPlayerFirst) v_ = -v_;
		}

		void SetValue(engine::Result result) {
			if (result == engine::kResultFirstPlayerWin) {
				v_ = 1.0f;
			}
			else if (result == engine::kResultSecondPlayerWin) {
				v_ = -1.0f;
			}
			else if (result == engine::kResultDraw) {
				v_ = 0.0f;
			}
			else {
				assert(false);
			}
		}

		float GetValue(state::PlayerSide view_side) {
			if (view_side == state::kPlayerFirst) return v_;
			else return -v_;
		}

	private:
		float v_;
	};
}