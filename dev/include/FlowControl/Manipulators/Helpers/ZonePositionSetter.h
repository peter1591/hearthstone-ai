#pragma once

#include "State/Cards/Card.h"

namespace state
{
	namespace board
	{
		class OrderedCardsManager;
	}
}

namespace FlowControl
{
	namespace Manipulators
	{
		namespace Helpers
		{
			// we separate this to another class to impose a finer access control
			class ZonePositionSetter
			{
				friend class state::board::OrderedCardsManager;

			public:
				ZonePositionSetter(state::Cards::Card & card) : card_(card) {}

			private: // only accessible to friend classes
				void Set(int pos)
				{
					card_.SetLocation().Position(pos);
				}

			private:
				state::Cards::Card & card_;
			};
		}
	}
}