#pragma once

#include "state/State.h"
#include "state/detail/InvokeCallback.h"
#include "engine/FlowControl/Manipulators/CardManipulator.h"

namespace state {
	namespace detail {
		template <CardType CardType>
		inline void InvokeCallback<CardType, kCardZonePlay>::Added(
			state::State & state, state::CardRef card_ref)
		{
			state.GetMutableCard(card_ref).SetJustPlayedFlag(true);
			state.GetMutableCard(card_ref).GetRawData()
				.added_to_play_zone(state::Cards::ZoneChangedContext
				{ state, card_ref});
		}

		template <CardType CardType>
		inline void InvokeCallback<CardType, kCardZoneHand>::Added(
			state::State & state, state::CardRef card_ref)
		{
			state.GetMutableCard(card_ref).GetRawData()
				.added_to_hand_zone(state::Cards::ZoneChangedContext{state, card_ref});
		}
	}
}