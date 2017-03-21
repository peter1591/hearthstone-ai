#pragma once

#include "FlowControl/enchantment/Enchantments.h"

namespace FlowControl
{
	namespace enchantment
	{
		inline void Enchantments::ApplyAll(state::Cards::EnchantableStates & card, state::State const& state)
		{
			enchantments_.IterateAll([&card, &state](ManagedEnchantment& item) -> bool {
				if (item.valid_until_turn > 0) {
					if (state.GetTurn() > item.valid_until_turn) return false;
				}
				item.apply_functor(card);
				return true;
			});
		}
	}
}