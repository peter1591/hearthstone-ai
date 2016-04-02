#pragma once

#include "game-engine/board-objects/minion.h"
#include "owner.h"

namespace GameEngine
{
	inline bool EnchantmentOwner::IsEmpty() const
	{
		return this->minion_enchantments.empty();
	}

	inline void EnchantmentOwner::RemoveOwnedEnchantments(Minion & owner)
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

	inline void EnchantmentOwner::EnchantmentAdded(
		Enchantment<Minion> * enchantment)
	{
		this->minion_enchantments.push_back(enchantment);
	}

	inline void EnchantmentOwner::EnchantmentRemoved(
		Enchantment<Minion> * enchantment)
	{
		for (auto it = this->minion_enchantments.begin(); it != this->minion_enchantments.end(); ) {
			if (*it == enchantment) it = this->minion_enchantments.erase(it);
			else it++;
		}
	}
} // namespace GameEngine