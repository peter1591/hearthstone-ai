#pragma once

#include <vector>
#include "EntitiesManager/EntitiesManager.h"
#include "EntitiesManager/CardRef.h"
#include "Entity/Card.h"
#include "State/Utils/OrderedCardsManager.h"

namespace State
{
	class Minions
	{
	public:
		Minions()
		{
			minions_.reserve(7);
		}

		Utils::OrderedCardsManager GetLocationManipulator()
		{
			return Utils::OrderedCardsManager(minions_);
		}

	private:
		std::vector<CardRef> minions_;
	};
}