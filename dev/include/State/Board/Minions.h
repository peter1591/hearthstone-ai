#pragma once

#include <vector>
#include "State/Cards/Manager.h"
#include "State/Cards/CardRef.h"
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

		size_t Size() const { return minions_.size(); }
		CardRef Get(size_t pos) const { return minions_[pos]; }

	private:
		std::vector<CardRef> minions_;
	};
}