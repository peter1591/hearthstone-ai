#pragma once

#include <vector>
#include "State/Cards/Manager.h"
#include "State/CardRef.h"
#include "State/Cards/Card.h"
#include "State/Utils/OrderedCardsManager.h"

namespace State
{
	class Graveyard
	{
	public:
		template <CardType ManipulatingCardType>
		Utils::OrderedCardsManager GetLocationManipulator()
		{
			return Utils::OrderedCardsManager(GetContainer<ManipulatingCardType>());
		}

		size_t GetTotalMinions() const { return minions_.size(); }
		size_t GetTotalSpells() const { return spells_.size(); }
		size_t GetTotalOthers() const { return others_.size(); }

	private:
		template <CardType ManipulatingCardType>
		std::vector<CardRef> & GetContainer();

	private:
		std::vector<CardRef> minions_;
		std::vector<CardRef> spells_;
		std::vector<CardRef> others_;
	};

	template <> std::vector<CardRef> & Graveyard::GetContainer<kCardTypeMinion>() { return minions_; }
	template <> std::vector<CardRef> & Graveyard::GetContainer<kCardTypeSpell>() { return spells_; }
	template <CardType ManipulatingCardType> std::vector<CardRef> & Graveyard::GetContainer() { return others_; }
}