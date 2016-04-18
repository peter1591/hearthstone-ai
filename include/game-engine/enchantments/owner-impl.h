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
	void EnchantmentsOwner<EnchantmentTarget>::EnchantmentAdded(OwnerItem const& managed_enchantment)
	{
		using OwnerToken = typename EnchantmentsOwner<EnchantmentTarget>::Token;

		OwnerToken token = this->enchantments.insert(this->enchantments.end(), managed_enchantment);
		OwnerItem & added_item = *token;

		added_item.Get()->SetOwner(this, token);
	}

	template <typename EnchantmentTarget>
	inline void EnchantmentsOwner<EnchantmentTarget>::EnchantmentRemoved(Token token)
	{
		this->enchantments.erase(token);
	}
} // namespace GameEngine