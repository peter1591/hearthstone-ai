#pragma once

#include "state/Cards/Card.h"

namespace FlowControl
{
	namespace Manipulators
	{
		namespace Helpers
		{
			template <state::CardType, state::CardZone> struct PlayerDataStructureMaintainer;

			// we separate this to another class to impose a finer access control
			class ZonePositionSetter
			{
				template <state::CardType, state::CardZone> friend struct PlayerDataStructureMaintainer;

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