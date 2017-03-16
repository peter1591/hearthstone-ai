#pragma once

#include "FlowControl/enchantment/TieredEnchantments.h"

namespace Cards
{
	using FlowControl::enchantment::EnchantmentTiers;

	class EnchantmentCardBase
	{
	public:
		FlowControl::enchantment::Enchantments::ApplyFunctor apply_functor;
	};
}