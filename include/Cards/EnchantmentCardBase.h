#pragma once

#include "state/Cards/Enchantments/Enchantments.h"

namespace Cards
{
	class EnchantmentCardBase
	{
	public:
		state::Cards::Enchantments::ApplyFunctor apply_functor;
	};
}