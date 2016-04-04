#pragma once

#include "game-engine/board-objects/minion.h"
#include "owner.h"

namespace GameEngine
{
	template <typename EnchantmentTarget>
	inline bool EnchantmentsOwner<typename EnchantmentTarget>::IsEmpty() const
	{
		return this->enchantments.empty();
	}

	template <typename EnchantmentTarget>
	inline void EnchantmentsOwner<EnchantmentTarget>::RemoveOwnedEnchantments()
	{
		while (!this->enchantments.empty()) {
			this->enchantments.begin()->Remove();
		}
	}

	template <typename EnchantmentTarget>
	inline void EnchantmentsOwner<typename EnchantmentTarget>::EnchantmentAdded(ManagedEnchantment<EnchantmentTarget> const& managed_enchantment)
	{
		this->enchantments.push_back(managed_enchantment);
	}

	template <typename EnchantmentTarget>
	inline void EnchantmentsOwner<typename EnchantmentTarget>::EnchantmentRemoved(ManagedItem managed_item)
	{
		for (auto it = this->enchantments.begin(); it != this->enchantments.end(); ++it)
		{
			if (!it->EqualsTo(managed_item)) continue;

			this->enchantments.erase(it);
			return;
		}
		throw std::runtime_error("cannot find enchantment to be removed");
	}
} // namespace GameEngine