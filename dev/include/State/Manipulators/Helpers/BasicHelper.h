#pragma once

#include "State/Cards/Card.h"

namespace FlowControl
{
	namespace Manipulators
	{
		namespace Helpers
		{
			class BasicHelper
			{
			public:
				static void SetCost(state::Cards::Card& card, int new_cost) { return card.SetCost(new_cost); }
			};
		}
	}
}