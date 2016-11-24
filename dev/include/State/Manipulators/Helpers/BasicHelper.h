#pragma once

#include "State/Cards/Card.h"

namespace State
{
	namespace Manipulators
	{
		namespace Helpers
		{
			class BasicHelper
			{
			public:
				static void SetCost(Cards::Card& card, int new_cost) { return card.SetCost(new_cost); }
			};
		}
	}
}