#pragma once

#include <vector>
#include "State/Types.h"
#include "State/Cards/Manager.h"
#include "State/Cards/Card.h"
#include "State/Utils/OrderedCardsManager.h"

namespace state
{
	namespace Manipulators
	{
		namespace Helpers
		{
			template <CardType TargetCardType, CardZone TargetCardZone> class AddToPlayerDatStructure;
			template <CardType TargetCardType, CardZone TargetCardZone> class RemoveFromPlayerDatStructure;
		}
	}

	namespace board
	{
		class Deck
		{
		public:
			class LocationManipulator
			{
				template <CardType TargetCardType, CardZone TargetCardZone> friend class state::Manipulators::Helpers::AddToPlayerDatStructure;
				template <CardType TargetCardType, CardZone TargetCardZone> friend class state::Manipulators::Helpers::RemoveFromPlayerDatStructure;

			public:
				explicit LocationManipulator(Deck & deck) : deck_(deck) {}

			private:
				template <typename T1, typename T2>
				void Insert(T1&& state, T2&& card_ref)
				{
					++deck_.change_id;
					auto instance = OrderedCardsManager(deck_.cards_);
					return instance.Insert(std::forward<T1>(state), std::forward<T2>(card_ref));
				}
				template <typename T1, typename T2>
				void Remove(T1&& state, T2&& pos)
				{
					++deck_.change_id;
					auto instance = OrderedCardsManager(deck_.cards_);
					return instance.Remove(std::forward<T1>(state), std::forward<T2>(pos));
				}

			private:
				Deck & deck_;
			};
			friend class LocationManipulator;

		public:
			Deck() : change_id(0)
			{
				cards_.reserve(40);
			}

			LocationManipulator GetLocationManipulator()
			{
				return LocationManipulator(*this);
			}

			CardRef Get(int idx) const { return cards_[idx]; }

			size_t Size() const { return cards_.size(); }
			bool Empty() const { return cards_.empty(); }

		private:
			int change_id;
			std::vector<CardRef> cards_;
		};
	}
}