#pragma once

#include "FlowControl/enchantment/Enchantments.h"

#include "state/State.h"
#include "FlowControl/enchantment/detail/UpdateDecider-impl.h"

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
	}
}