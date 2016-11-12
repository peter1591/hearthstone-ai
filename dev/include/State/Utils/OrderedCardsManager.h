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
		public:
			typedef std::vector<CardRef> ContainerType;

			explicit OrderedCardsManager(ContainerType & container) : container_(container) {}

			void Insert(EntitiesManager & mgr, CardRef card_ref);
			void Remove(EntitiesManager & mgr, size_t pos);

		private:
			ContainerType & container_;
		};
	}
}