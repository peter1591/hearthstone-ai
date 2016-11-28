#pragma once

#include <vector>
#include "State/Types.h"
#include "State/Cards/Manager.h"

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
		class Hand
		{
			friend class FlowControl::Manipulators::Helpers::OrderedCardsManager;

		public:
			Hand()
			{
				cards_.reserve(max_cards_);
			}

			size_t Size() const { return cards_.size(); }
			bool Empty() const { return cards_.empty(); }
			bool Full() const { return Size() >= max_cards_; }

			CardRef Get(int idx) { return cards_[idx]; }

		private:
			static constexpr int max_cards_ = 10;
			std::vector<CardRef> cards_;
		};
	}
}