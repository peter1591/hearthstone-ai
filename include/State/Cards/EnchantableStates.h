#pragma once

#include "State/Types.h"

namespace state
{
	namespace Cards
	{
		class EnchantableStates
		{
		public:
			EnchantableStates() : cost(-1), attack(-1), max_hp(-1), taunt(false), shielded(false) {}

			PlayerIdentifier player;
			int cost;
			int attack;
			int max_hp;

			bool taunt;
			bool shielded;

			static constexpr int kFieldChangeId = 3; // Change this if any field is changed. This helps to see where you should also modify
		};
	}
}