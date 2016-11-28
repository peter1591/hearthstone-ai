#pragma once

#include "State/Cards/Card.h"
#include "FlowControl/Manipulators/Helpers/EnchantmentHelper.h"
#include "FlowControl/Manipulators/Helpers/AuraHelper.h"
#include "FlowControl/Manipulators/Helpers/ZonePositionSetter.h"
#include "FlowControl/Manipulators/Helpers/ZoneChanger.h"
#include "State/State.h"

namespace FlowControl
{
	namespace Manipulators
	{
		class CardManipulator
		{
		public:
			CardManipulator(state::State & state, CardRef card_ref, state::Cards::Card &card) :
				state_(state), card_ref_(card_ref), card_(card)
			{
			}

		public:
			void Cost(int new_cost) { card_.SetCost(new_cost); }

			Helpers::EnchantmentHelper Enchant() { return Helpers::EnchantmentHelper(card_); }
			Helpers::AuraHelper Aura() { return Helpers::AuraHelper(state_, card_); }
			Helpers::ZonePositionSetter ZonePosition() { return Helpers::ZonePositionSetter(card_); }

			Helpers::ZoneChangerWithUnknownZoneUnknownType Zone()
			{
				// TODO: Can specialize the zone changer to accelerate when moving from a non-play zone to another non-play zone
				// For example: deck --> hand
				return Helpers::ZoneChangerWithUnknownZoneUnknownType(state_, card_ref_, card_);
			}

		protected:
			state::State & state_;
			CardRef card_ref_;
			state::Cards::Card & card_;
		};
	}
}