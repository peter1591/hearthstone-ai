#pragma once

#include "State/Cards/Card.h"
#include "Manipulators/Helpers/BasicHelper.h"
#include "Manipulators/Helpers/EnchantmentHelper.h"
#include "Manipulators/Helpers/AuraHelper.h"
#include "Manipulators/Helpers/ZonePositionSetter.h"
#include "Manipulators/Helpers/ZoneChanger.h"
#include "State/State.h"

namespace Manipulators
{
	class MinionManipulator
	{
	public:
		MinionManipulator(State::State & state, CardRef card_ref, State::Cards::Card &card) :
			state_(state), card_ref_(card_ref), card_(card)
		{
		}

		void SetCost(int new_cost) { Helpers::BasicHelper::SetCost(card_, new_cost); }

	public:
		Helpers::EnchantmentHelper Enchant() { return Helpers::EnchantmentHelper(card_); }
		Helpers::AuraHelper Aura() { return Helpers::AuraHelper(state_, card_); }
		Helpers::ZonePositionSetter ZonePosition() { return Helpers::ZonePositionSetter(card_); }
		Helpers::ZoneChangerWithUnknownZone<State::kCardTypeMinion> GetZoneChanger() {
			return Helpers::ZoneChangerWithUnknownZone<State::kCardTypeMinion>(state_.mgr, card_ref_, card_);
		}

	private:
		State::State & state_;
		CardRef card_ref_;
		State::Cards::Card & card_;
	};
}