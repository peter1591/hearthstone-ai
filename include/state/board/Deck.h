#pragma once

#include <algorithm>
#include <vector>
#include "state/Types.h"
#include "state/Cards/Manager.h"
#include "state/Cards/Card.h"

namespace FlowControl
{
	namespace Manipulators
	{
		namespace Helpers
		{
			template <state::CardType TargetCardType, state::CardZone TargetCardZone> class AddToPlayerDatStructure;
			template <state::CardType TargetCardType, state::CardZone TargetCardZone> class RemoveFromPlayerDatStructure;
			class OrderedCardsManager;
		}
	}
}

namespace state
{
	namespace board
	{
		class Deck
		{
		public:
			Deck() : change_id(0)
			{
				cards_.reserve(40);
			}

			CardRef GetLast() const { return cards_.back(); }

			size_t Size() const { return cards_.size(); }
			bool Empty() const { return cards_.empty(); }

			template <typename RandomGenerator>
			void ShuffleAdd(CardRef card, RandomGenerator&& rand)
			{
				++change_id;
				auto it_new = cards_.insert(cards_.end(), card);

				if (cards_.size() <= 1) return;
				auto rand_idx = rand(cards_.size());
				std::iter_swap(cards_.begin() + rand_idx, it_new);
			}

			void RemoveLast()
			{
				++change_id;
				cards_.pop_back();
			}

		private:
			int change_id;
			std::vector<CardRef> cards_;
		};
	}
}