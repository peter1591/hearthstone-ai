#pragma once

#include "game-engine/board-objects/minion.h"
#include "owner.h"

namespace GameEngine
{
	template <typename EnchantmentTarget>
	inline bool EnchantmentOwner<typename EnchantmentTarget>::IsEmpty() const
	{
		return this->minion_enchantments.empty();
	}

	template <>
	inline void EnchantmentOwner<Minion>::RemoveOwnedEnchantments(Minion & owner)
	{
		// remove enchantment from minions
		while (!this->minion_enchantments.empty()) {
			Enchantment<Minion> * removing_enchant = this->minion_enchantments.front();

			for (auto it = owner.GetBoard().object_manager.GetMinionIteratorAtBeginOfSide(SLOT_PLAYER_SIDE); !it.IsEnd(); it.GoToNext()) {
				it->RemoveEnchantment(removing_enchant);
			}
			for (auto it = owner.GetBoard().object_manager.GetMinionIteratorAtBeginOfSide(SLOT_OPPONENT_SIDE); !it.IsEnd(); it.GoToNext()) {
				it->RemoveEnchantment(removing_enchant);
			}
		}
	}

	template <typename EnchantmentTarget>
	inline void EnchantmentOwner<typename EnchantmentTarget>::EnchantmentAdded(Enchantment<EnchantmentTarget> * enchantment)
	{
		this->minion_enchantments.push_back(enchantment);
	}

	template <typename EnchantmentTarget>
	inline void EnchantmentOwner<typename EnchantmentTarget>::EnchantmentRemoved(Enchantment<EnchantmentTarget> * enchantment)
	{
		for (auto it = this->minion_enchantments.begin(); it != this->minion_enchantments.end(); ) {
			if (*it == enchantment) it = this->minion_enchantments.erase(it);
			else it++;
		}
	}
} // namespace GameEngine