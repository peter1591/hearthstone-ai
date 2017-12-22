#pragma once

#include <assert.h>
#include "state/Cards/Card.h"
#include "engine/FlowControl/Manipulators/CardManipulator.h"

namespace engine {
	namespace FlowControl
	{
		namespace Manipulators
		{
			class WeaponManipulator : public CardManipulator
			{
			public:
				WeaponManipulator(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref)
					: CardManipulator(state, flow_context, card_ref)
				{
					assert(GetCard().GetCardType() == state::kCardTypeWeapon);
				}
			};
		}
	}
}