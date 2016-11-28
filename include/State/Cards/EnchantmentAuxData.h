#pragma once

#include "State/Cards/EnchantableStates.h"
#include "Enchantments/TieredEnchantments.h"

namespace state
{
	namespace Cards
	{
		class EnchantmentAuxData
		{
		public:
			EnchantmentAuxData() : need_update(true) {}

			TieredEnchantments enchantments;
			EnchantableStates origin_states;
			bool need_update;
		};
	}
}