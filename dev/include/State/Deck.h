#pragma once

#include <vector>
#include "EntitiesManager/EntitiesManager.h"
#include "EntitiesManager/CardRef.h"
#include "Entity/Card.h"
#include "State/Utils/OrderedCardsManager.h"

namespace State
{
	class Deck
	{
	public:
		Deck()
		{
			cards_.reserve(40);
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