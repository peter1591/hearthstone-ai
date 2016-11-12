#pragma once

#include <vector>
#include "EntitiesManager/EntitiesManager.h"
#include "EntitiesManager/CardRef.h"
#include "Entity/CardType.h"

namespace Manipulators
{
	namespace Helpers
	{
		template <Entity::CardType T> class ZoneChanger;
	}
}

namespace State
{
	class Hand
	{
	public:
		class LocationManipualtor
		{
			friend class Hand;
			template <Entity::CardType T> friend class Manipulators::Helpers::ZoneChanger;

		private:
			explicit LocationManipualtor(std::vector<CardRef> & cards) : cards_(cards) {}

			void Insert(EntitiesManager & mgr, CardRef card_ref);
			void Remove(EntitiesManager & mgr, int pos);

		private:
			std::vector<CardRef> & cards_;
		};

	public:
		Hand()
		{
			cards_.reserve(10);
		}

		LocationManipualtor GetLocationManipulator()
		{
			return LocationManipualtor(cards_);
		}

	private:
		std::vector<CardRef> cards_;
	};
}