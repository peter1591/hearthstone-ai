#pragma once

#include <vector>
#include "State/Types.h"
#include "State/Cards/Manager.h"
#include "State/board/OrderedCardsManager.h"

namespace state
{
	namespace board
	{
		class Hand
		{
		public:
			Hand()
			{
				cards_.reserve(max_cards_);
			}

			OrderedCardsManager GetLocationManipulator()
			{
				return OrderedCardsManager(cards_);
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