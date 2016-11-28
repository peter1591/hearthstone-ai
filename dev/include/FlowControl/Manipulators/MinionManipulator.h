#pragma once

#include "State/State.h"
#include "State/Cards/Card.h"
#include "FlowControl/Manipulators/CharacterManipulator.h"

namespace FlowControl
{
	namespace Manipulators
	{
		class MinionManipulator : public CharacterManipulator
		{
		public:
			MinionManipulator(state::State & state, state::CardRef card_ref, state::Cards::Card &card)
				: CharacterManipulator(state, card_ref, card)
			{
			}

			Helpers::ZoneChangerWithUnknownZone<state::kCardTypeMinion> Zone()
			{
				return Helpers::ZoneChangerWithUnknownZone<state::kCardTypeMinion>(state_, card_ref_, card_);
			}
		};
	}
}