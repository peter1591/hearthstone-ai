#pragma once

#include "state/detail/InvokeCallback.h"
#include "FlowControl/Manipulators/CardManipulator.h"

namespace state {
	namespace detail {
		template <CardType CardType>
		inline void InvokeCallback<CardType, kCardZonePlay>::Added(
			FlowControl::Manipulate const& manipulate, state::Events::Manager & event_mgr, state::CardRef card_ref)
		{
			manipulate.Card(card_ref).AfterAddedToPlayZone(event_mgr);
		}
		template <CardType CardType>
		inline void InvokeCallback<CardType, kCardZoneHand>::Added(
			FlowControl::Manipulate const& manipulate, state::Events::Manager & event_mgr, state::CardRef card_ref)
		{
			manipulate.Card(card_ref).AfterAddedToHandZone(event_mgr);
		}
	}
}