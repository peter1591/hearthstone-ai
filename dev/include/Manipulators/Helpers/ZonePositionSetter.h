#pragma once

#include "Entity/Card.h"

namespace State
{
	class Minions;
}

namespace Manipulators
{
	namespace Helpers
	{
		// we separate this to another class to impose a finer access control
		class ZonePositionSetter
		{
			friend class State::Minions;

		public:
			ZonePositionSetter(Entity::Card & card) : card_(card) {}

		private: // only accessible to friend classes
			void SetZonePosition(int pos)
			{
				card_.SetZonePosition(pos);
			}

		private:
			Entity::Card & card_;
		};
	}
}