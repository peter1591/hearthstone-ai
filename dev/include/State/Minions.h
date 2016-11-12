#pragma once

#include <vector>
#include "EntitiesManager/EntitiesManager.h"
#include "EntitiesManager/CardRef.h"
#include "Entity/Card.h"

namespace Manipulators
{
	namespace Helpers
	{
		template <Entity::CardType T> class ZoneChanger;
	}
}

namespace State
{
	class Minions
	{
	public:
		class LocationManipualtor
		{
			template <Entity::CardType T> friend class Manipulators::Helpers::ZoneChanger;

		public:
			explicit LocationManipualtor(std::vector<CardRef> & minions) : minions_(minions) {}

		private:
			void Insert(EntitiesManager & mgr, CardRef card_ref);
			void Remove(EntitiesManager & mgr, int pos);

		private:
			std::vector<CardRef> & minions_;
		};

	public:
		Minions()
		{
			minions_.reserve(7);
		}

		LocationManipualtor GetLocationManipulator()
		{
			return LocationManipualtor(minions_);
		}

	private:
		std::vector<CardRef> minions_;
	};
}