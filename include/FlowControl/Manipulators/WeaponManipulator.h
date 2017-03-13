#pragma once

#include <assert.h>
#include "state/Cards/Card.h"
#include "FlowControl/Manipulators/CardManipulator.h"

namespace FlowControl
{
	namespace Manipulators
	{
		class WeaponManipulator : public CardManipulator
		{
		public:
			WeaponManipulator(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card &card)
				: CardManipulator(state, flow_context, card_ref, card)
			{
				assert(card.GetCardType() == state::kCardTypeWeapon);
			}

			Helpers::DamageHelper Damage(int amount) { return Helpers::DamageHelper(state_, flow_context_, card_ref_, card_, amount); }
		};
	}
}