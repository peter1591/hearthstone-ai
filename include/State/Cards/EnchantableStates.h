#pragma once

#include "State/Types.h"

namespace state
{
	namespace Cards
	{
		class EnchantableStates
		{
		public:
			EnchantableStates() : zone(kCardZoneInvalid), cost(-1), attack(-1), max_hp(-1) {}

			PlayerIdentifier player;
			CardZone zone;
			int cost;
			int attack;
			int max_hp;
		};
	}
}