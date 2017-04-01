#pragma once

#include "FlowControl/enchantment/Enchantments.h"
#include "state/State.h"

namespace FlowControl {
	namespace enchantment {
		inline bool Enchantments::NormalEnchantment::Apply(state::State const& state, state::Cards::EnchantableStates & stats) {
			if (valid_until_turn > 0) {
				if (state.GetTurn() > valid_until_turn) {
					return false;
				}
			}
			apply_functor(stats);
			return true;
		}
	}
}