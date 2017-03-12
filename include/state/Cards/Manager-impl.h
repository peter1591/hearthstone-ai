#pragma once

#include "state/Cards/Manager.h"

#include "FlowControl/Manipulate.h"
#include "FlowControl/Manipulators/CharacterManipulator.h"
#include "FlowControl/Manipulators/MinionManipulator.h"
#include "state/State.h"

namespace state
{
	// TODO: remove 'state', and 'flow_context'
	inline CardRef Cards::Manager::PushBack(State & state, state::FlowContext & flow_context, Cards::Card&& card)
	{
		assert(this == &state.mgr);
		assert(card.GetZone() == kCardZoneInvalid);
		CardRef ref = CardRef(cards_.PushBack(std::move(card)));
		return ref;
	}
}