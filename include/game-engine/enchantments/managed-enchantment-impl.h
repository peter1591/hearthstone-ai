#pragma once

#include "managed-enchantment.h"

namespace GameEngine
{
	template<typename Owner>
	inline ManagedEnchantment<Owner>::ManagedEnchantment(Owner & owner, std::unique_ptr<EnchantmentType>&& enchantment)
		: owner(owner), enchantment(std::move(enchantment))
	{
	}

} // namespace GameEngine