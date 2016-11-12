#pragma once

#include <vector>
#include "EntitiesManager/EntitiesManager.h"
#include "EntitiesManager/CardRef.h"
#include "Entity/Card.h"

// The zone position will not be set

namespace State
{
	class Graveyard
	{
	public:
		template <Entity::CardType ManipulatingCardType>
		void PushBack(EntitiesManager & mgr, CardRef card_ref)
		{
			GetContainer<ManipulatingCardType>().push_back(card_ref);
		}

		template <Entity::CardType ManipulatingCardType>
		void Remove(EntitiesManager & mgr, int idx)
		{
			auto & container = GetContainer<ManipulatingCardType>();
			container.erase(container.begin() + idx);
		}

		size_t GetTotalMinions() const { return minions_.size(); }
		size_t GetTotalSpells() const { return spells_.size(); }

	private:
		template <Entity::CardType ManipulatingCardType>
		std::vector<CardRef> & GetContainer();

	private:
		std::vector<CardRef> minions_;
		std::vector<CardRef> spells_;
		std::vector<CardRef> others_;
	};

	template <> std::vector<CardRef> & Graveyard::GetContainer<Entity::kCardTypeMinion>() { return minions_; }
	template <> std::vector<CardRef> & Graveyard::GetContainer<Entity::kCardTypeSpell>() { return spells_; }
	template <Entity::CardType ManipulatingCardType> std::vector<CardRef> & Graveyard::GetContainer() { return others_; }
}