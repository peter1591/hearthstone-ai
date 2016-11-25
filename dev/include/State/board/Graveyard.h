#pragma once

#include <vector>
#include "State/Types.h"
#include "State/Cards/Manager.h"
#include "State/Cards/Card.h"
#include "State/board/OrderedCardsManager.h"

namespace state
{
	namespace board
	{
		class Graveyard
		{
		public:
			template <CardType ManipulatingCardType>
			OrderedCardsManager GetLocationManipulator()
			{
				return OrderedCardsManager(GetContainer<ManipulatingCardType>());
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
}