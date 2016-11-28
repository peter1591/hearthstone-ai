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
			MinionManipulator(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card &card)
				: CharacterManipulator(state, flow_context, card_ref, card)
			{
				assert(card.GetCardType() == state::kCardTypeMinion);
			}

			Helpers::ZoneChangerWithUnknownZone<state::kCardTypeMinion> Zone()
			{
				return Helpers::ZoneChangerWithUnknownZone<state::kCardTypeMinion>(state_, flow_context_, card_ref_, card_);
			}
		};
	}
}