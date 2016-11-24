#pragma once

#include <vector>
#include "State/Cards/Manager.h"
#include "State/Cards/CardRef.h"
#include "Entity/CardType.h"
#include "State/Utils/OrderedCardsManager.h"

namespace State
{
	class Hand
	{
	public:
		Hand()
		{
			cards_.reserve(max_cards_);
		}

		Utils::OrderedCardsManager GetLocationManipulator()
		{
			return Utils::OrderedCardsManager(cards_);
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