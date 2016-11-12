#pragma once

#include <vector>
#include "EntitiesManager/CardRef.h"
#include "EntitiesManager/EntitiesManager.h"

namespace Manipulators
{
	namespace Helpers
	{
		template <Entity::CardType T> class ZoneChanger;
	}
}

namespace State
{
	namespace Utils
	{
		class OrderedCardsManager
		{
			template <Entity::CardType T> friend class Manipulators::Helpers::ZoneChanger;

		public:
			typedef std::vector<CardRef> ContainerType;

			explicit OrderedCardsManager(ContainerType & container) : container_(container) {}

		private:
			void Insert(EntitiesManager & mgr, CardRef card_ref);
			void Remove(EntitiesManager & mgr, int pos);

		private:
			ContainerType & container_;
		};
	}
}