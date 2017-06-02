#pragma once

#include "state/detail/InvokeCallback.h"
#include "FlowControl/Manipulators/CardManipulator.h"

namespace state {
	namespace detail {
		template <CardType CardType>
		inline void InvokeCallback<CardType, kCardZonePlay>::Added(
			FlowControl::Manipulate const& manipulate, state::State & state, state::CardRef card_ref)
		{
			manipulate.Card(card_ref).AfterAddedToPlayZone(state.event_mgr_);
		}
		template <CardType CardType>
		inline void InvokeCallback<CardType, kCardZoneHand>::Added(
			FlowControl::Manipulate const& manipulate, state::State & state, state::CardRef card_ref)
		{
			manipulate.Card(card_ref).AfterAddedToHandZone(state.event_mgr_);
		}
	}
}
