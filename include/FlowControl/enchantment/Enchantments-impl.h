#pragma once

#include "FlowControl/enchantment/Enchantments.h"

namespace FlowControl
{
	namespace enchantment
	{
		inline void Enchantments::ApplyAll(state::Cards::EnchantableStates & card, state::State const& state)
		{
			for (auto it = enchantments_.begin(); it != enchantments_.end();) {
				if (it->valid_until_turn > 0) {
					if (state.GetTurn() > it->valid_until_turn) {
						it = enchantments_.erase(it);
						continue;
					}
				}
				it->apply_functor(card);
				++it;
			}
		}
	}
}