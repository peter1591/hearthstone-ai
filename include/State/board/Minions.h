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
		class Minions
		{
			friend class FlowControl::Manipulators::Helpers::OrderedCardsManager;

		public:
			Minions()
			{
				minions_.reserve(7);
			}

			size_t Size() const { return minions_.size(); }
			CardRef Get(size_t pos) const { return minions_[pos]; }
			bool Full() const { return Size() >= 7; }

			std::vector<CardRef> const& Get() const { return minions_; }

		private:
			std::vector<CardRef> minions_;
		};
	}
}