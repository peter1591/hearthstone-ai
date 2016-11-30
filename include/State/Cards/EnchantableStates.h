#pragma once

#include "State/Types.h"

namespace state
{
	namespace Cards
	{
		class EnchantableStates
		{
		public:
			EnchantableStates() : cost(-1), attack(-1), max_hp(-1) {}

			PlayerIdentifier player;
			int cost;
			int attack;
			int max_hp;
		};
	}
}