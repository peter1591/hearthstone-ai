#pragma once

#include <vector>
#include "State/Types.h"
#include "State/Cards/Manager.h"
#include "State/Cards/Card.h"

namespace FlowControl
{
	namespace Manipulators
	{
		namespace Helpers
		{
			class OrderedCardsManager;
		}
	}
}

namespace state
{
	namespace board
	{
		class Graveyard
		{
			friend class FlowControl::Manipulators::Helpers::OrderedCardsManager;

		public:
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
	}
}