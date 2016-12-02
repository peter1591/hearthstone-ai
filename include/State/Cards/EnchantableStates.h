#pragma once

#include "State/Types.h"
#include "State/Cards/Mechanics.h"

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

			Mechanics mechanics;

			static constexpr int kFieldChangeId = 1; // Change this if any field is changed. This helps to see where you should also modify
		};
	}
}