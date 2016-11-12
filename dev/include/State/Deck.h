#pragma once

#include <vector>
#include "EntitiesManager/EntitiesManager.h"
#include "EntitiesManager/CardRef.h"
#include "Entity/Card.h"

// The zone position will not be set

namespace State
{
	class Deck
	{
	public:
		Deck()
		{
			cards_.reserve(40);
		}

		void PushBack(EntitiesManager & mgr, CardRef card_ref)
		{
			cards_.push_back(card_ref);
		}

		void Remove(EntitiesManager & mgr, int idx)
		{
			cards_.erase(cards_.begin() + idx);
		}

		size_t Size() const { return cards_.size(); }

	private:
		std::vector<CardRef> cards_;
	};
}