#pragma once

#include "state/Cards/EnchantableStates.h"
#include "state/Cards/Enchantments/TieredEnchantments.h"

namespace state
{
	namespace Cards
	{
		class EnchantmentAuxData
		{
		public:
			EnchantmentAuxData() : need_update(true) {}

			template <typename EnchantmentType, typename T>
			bool Exists(T&& id) const
			{
				return enchantments.Exists<EnchantmentType>(std::forward<T>(id));
			}

			EnchantableStates origin_states;
			TieredEnchantments enchantments;
			bool need_update;
		};
	}
}