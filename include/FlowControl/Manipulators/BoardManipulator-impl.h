#pragma once

#include "state/Types.h"
#include "state/State.h"
#include "FlowControl/Manipulators/BoardManipulator.h"

namespace FlowControl
{
	namespace Manipulators
	{
		inline void BoardManipulator::Summon(state::CardRef ref)
		{
			state::Cards::Card const& card = state_.mgr.Get(ref);

			assert(card.GetPlayerIdentifier() != state::kPlayerInvalid);
			assert(card.GetZone() == state::kCardZonePlay);
			assert(card.GetCardType() == state::kCardTypeMinion);
			assert(card.GetZonePosition() >= 0);
			assert(card.GetZonePosition() <= 6);

			state_.event_mgr.TriggerEvent<state::Events::EventTypes::BeforeMinionSummoned>(
				state::Events::EventTypes::BeforeMinionSummoned::Context{ state_, ref, card });

			if (card.GetRawData().added_to_play_zone) {
				(*card.GetRawData().added_to_play_zone)({ state_, flow_context_, ref, card });
			}

			state_.event_mgr.TriggerEvent<state::Events::EventTypes::AfterMinionSummoned>();
			Manipulate(state_, flow_context_).Minion(ref).AfterSummoned();
		}
	}
}