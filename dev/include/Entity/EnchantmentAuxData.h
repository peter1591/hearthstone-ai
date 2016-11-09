#pragma once

#include "Entity/EnchantableStates.h"
#include "Enchantments/TieredEnchantments.h"

namespace Entity
{
	struct EnchantmentAuxData
	{
		TieredEnchantments enchantments;
		EnchantableStates origin_states;
		bool need_update;
	};
}