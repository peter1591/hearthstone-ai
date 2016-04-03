#pragma once

#include "game-engine/board-objects/minion.h"
#include "owner.h"

namespace GameEngine
{
	template <typename EnchantmentTarget>
	inline bool EnchantmentOwner<typename EnchantmentTarget>::IsEmpty() const
	{
		return this->enchantments.empty();
	}

	template <>
	inline void EnchantmentOwner<Minion>::RemoveOwnedEnchantments()
	{
		for (auto & item : this->enchantments) { item.Remove(); }
	}

	template <typename EnchantmentTarget>
	inline void EnchantmentOwner<typename EnchantmentTarget>::EnchantmentAdded(ManagedEnchantment<EnchantmentTarget> const& managed_enchantment)
	{
		this->enchantments.push_back(managed_enchantment);
	}

	template <typename EnchantmentTarget>
	inline void EnchantmentOwner<typename EnchantmentTarget>::EnchantmentRemoved(
		EnchantmentsItemType<EnchantmentTarget> const& managed_enchantment)
	{
		for (auto it = this->enchantments.begin(); it != this->enchantments.end(); )
		{
			typename std::list<ManagedEnchantment<EnchantmentTarget>>::const_iterator it_const = it;
			if (*it_const->Get() != managed_enchantment) continue;

			this->enchantments.erase(it);
			break;
		}
	}
} // namespace GameEngine