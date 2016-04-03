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
		while (!this->enchantments.empty()) {
			this->enchantments.begin()->Remove();
		}
	}

	template <typename EnchantmentTarget>
	inline void EnchantmentOwner<typename EnchantmentTarget>::EnchantmentAdded(ManagedEnchantment<EnchantmentTarget> const& managed_enchantment)
	{
		this->enchantments.push_back(managed_enchantment);
	}

	template <typename EnchantmentTarget>
	inline void EnchantmentOwner<typename EnchantmentTarget>::EnchantmentRemoved(ManagedItem managed_item)
	{
		for (auto it = this->enchantments.begin(); it != this->enchantments.end(); ++it)
		{
			typename std::list<ManagedEnchantment<EnchantmentTarget>>::const_iterator it_const = it;
			if (it_const->Get() != managed_item) continue;

			this->enchantments.erase(it);
			return;
		}
		throw std::runtime_error("cannot find enchantment to be removed");
	}
} // namespace GameEngine