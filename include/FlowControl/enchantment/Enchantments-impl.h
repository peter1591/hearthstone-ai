#pragma once

#include "FlowControl/enchantment/Enchantments.h"
#include "state/State.h"

namespace FlowControl {
	namespace enchantment {
		inline bool Enchantments::NormalEnchantment::Apply(ApplyFunctorContext const& context) const {
			if (valid_until_turn > 0) {
				if (context.state_.GetTurn() > valid_until_turn) {
					return false;
				}
			}
			apply_functor(context);
			return true;
		}

		inline void Enchantments::NormalEnchantmentUpdateDecider::PushBack(int valid_until_turn) {
			if (valid_until_turn < 0) return;
			if (need_update_after_turn_ < 0) {
				need_update_after_turn_ = valid_until_turn;
				return;
			}
			assert(need_update_after_turn_ == valid_until_turn); // provided that all enchantment are 'valid until the end of the turn'
		}

		inline void Enchantments::NormalEnchantmentUpdateDecider::FinishedUpdate(state::State const & state)
		{
			if (need_update_after_turn_ < 0) return;
			if (state.GetTurn() > need_update_after_turn_) {
				need_update_after_turn_ = -1;
			}
		}

		inline bool Enchantments::NormalEnchantmentUpdateDecider::NeedUpdate(state::State const & state) const
		{
			if (need_update_after_turn_ < 0) return false;

			if (state.GetTurn() > need_update_after_turn_) {
				return true;
			}
			return false;
		}

	}
}