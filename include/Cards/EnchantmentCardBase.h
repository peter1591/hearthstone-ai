#pragma once

#include "state/Cards/Enchantments/Enchantments.h"

namespace Cards
{
	class EnchantmentCardBase
	{
	public:
		EnchantmentCardBase() : after_added_callback(nullptr) {}

		state::Cards::Enchantments::ApplyFunctor apply_functor;
		state::Cards::Enchantments::AfterAddedCallback *after_added_callback;
	};
}