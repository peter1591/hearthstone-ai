#pragma once
#include "FlowControl/Manipulators/CardManipulator.h"

#include "state/State.h"

namespace FlowControl
{
	namespace Manipulators
	{
		inline void MinionManipulator::Silence()
		{
			// No need to clear emitting aura. In next AuraUpdate(), the silenced flag is checked.

			// Remove all enchantments, including the aura enchantments coming from other minions.
			// Those aura enchantments will be added back in the next AuraUpdate()
			card_.GetMutableEnchantmentHandler().Clear();

			// Remove all deathrattles
			card_.GetMutableDeathrattleHandler().Clear();

			card_.SetSilenced();
		}
	}
}