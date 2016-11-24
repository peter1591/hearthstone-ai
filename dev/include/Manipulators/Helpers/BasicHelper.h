#pragma once

#include "State/Cards/Card.h"

namespace Manipulators
{
	namespace Helpers
	{
		class BasicHelper
		{
		public:
			static void SetCost(Entity::Card& card, int new_cost) { return card.SetCost(new_cost); }
		};
	}
}