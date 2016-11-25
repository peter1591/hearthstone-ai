#pragma once

#include <vector>
#include "State/Types.h"
#include "State/Cards/Manager.h"
#include "State/Cards/Card.h"
#include "State/Utils/OrderedCardsManager.h"

namespace state
{
	namespace board
	{
		class Minions
		{
		public:
			Minions()
			{
				minions_.reserve(7);
			}

			OrderedCardsManager GetLocationManipulator()
			{
				return OrderedCardsManager(minions_);
			}

			size_t Size() const { return minions_.size(); }
			CardRef Get(size_t pos) const { return minions_[pos]; }

		private:
			std::vector<CardRef> minions_;
		};
	}
}