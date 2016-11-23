#pragma once

#include "Entity/Card.h"
#include "Manipulators/Helpers/BasicHelper.h"
#include "Manipulators/Helpers/EnchantmentHelper.h"
#include "Manipulators/Helpers/AuraHelper.h"
#include "Manipulators/Helpers/ZonePositionSetter.h"
#include "Manipulators/Helpers/ZoneChanger.h"
#include "State/State.h"

class EntitiesManager;

namespace Manipulators
{
	class CardManipulator
	{
	public:
		CardManipulator(State::State & state, CardRef card_ref, Entity::Card &card) :
			state_(state), card_ref_(card_ref), card_(card)
		{
		}

		void SetCost(int new_cost) { Helpers::BasicHelper::SetCost(card_, new_cost); }

	public:
		Helpers::EnchantmentHelper Enchant() { return Helpers::EnchantmentHelper(card_); }
		Helpers::AuraHelper Aura() { return Helpers::AuraHelper(state_, card_); }
		Helpers::ZonePositionSetter ZonePosition() { return Helpers::ZonePositionSetter(card_); }

		Helpers::ZoneChangerWithUnknownZoneUnknownType GetZoneChanger()
		{
			// TODO: Can specialize the zone changer to accelerate when moving from a non-play zone to another non-play zone
			// For example: deck --> hand
			return Helpers::ZoneChangerWithUnknownZoneUnknownType(state_.mgr, card_ref_, card_);
		}

	private:
		State::State & state_;
		CardRef card_ref_;
		Entity::Card & card_;
	};
}