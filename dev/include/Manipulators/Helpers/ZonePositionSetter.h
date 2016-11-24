#pragma once

#include "State/Cards/Card.h"

namespace State
{
	namespace Utils
	{
		class OrderedCardsManager;
	}
}

namespace Manipulators
{
	namespace Helpers
	{
		// we separate this to another class to impose a finer access control
		class ZonePositionSetter
		{
			friend class State::Utils::OrderedCardsManager;

		public:
			ZonePositionSetter(Entity::Card & card) : card_(card) {}

		private: // only accessible to friend classes
			void Set(int pos)
			{
				card_.SetLocation().Position(pos);
			}

		private:
			Entity::Card & card_;
		};
	}
}