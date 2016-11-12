#pragma once

#include <vector>
#include "EntitiesManager/EntitiesManager.h"
#include "EntitiesManager/CardRef.h"
#include "Entity/CardType.h"
#include "State/Utils/OrderedCardsManager.h"

namespace State
{
	class Hand
	{
	public:
		Hand()
		{
			cards_.reserve(10);
		}

		Utils::OrderedCardsManager GetLocationManipulator()
		{
			return Utils::OrderedCardsManager(cards_);
		}

		size_t Size() const { return cards_.size(); }

	private:
		std::vector<CardRef> cards_;
	};
}