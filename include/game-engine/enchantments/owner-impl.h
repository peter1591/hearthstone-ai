#pragma once

#include "game-engine/enchantments/types.h"
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
		using OwnerItem = typename EnchantmentTypes<EnchantmentTarget>::OwnerItem;
		using ManagerManagedItem = typename EnchantmentTypes<EnchantmentTarget>::ManagerManagedItem;

		while (!this->enchantments.empty())
		{
			OwnerItem owner_item = this->enchantments.front(); // copy out

			this->RemoveEnchantment(this->enchantments.begin()); // remove from owner enchantments
			owner_item.Get()->UnsetOwner();

			owner_item.GetManager().Remove(owner_item.Get()); // remove from enchantment manager (a.k.a., Enchantments)
		}
	}

	template <typename EnchantmentTarget>
	void EnchantmentsOwner<EnchantmentTarget>::AddEnchantment(OwnerItem const& managed_enchantment)
	{
		using OwnerToken = typename EnchantmentsOwner<EnchantmentTarget>::Token;

		OwnerToken token = this->enchantments.insert(this->enchantments.end(), managed_enchantment);
		OwnerItem & added_item = *token;

		added_item.Get()->SetOwner(this, token);
	}

	template<typename EnchantmentTarget>
	inline void EnchantmentsOwner<EnchantmentTarget>::RemoveEnchantment(Token token)
	{
		this->enchantments.erase(token);
	}
} // namespace GameEngine