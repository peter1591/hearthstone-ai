#pragma once

#include "state/Cards/Manager.h"

#include "FlowControl/Manipulate.h"
#include "FlowControl/Manipulators/CharacterManipulator.h"
#include "FlowControl/Manipulators/MinionManipulator.h"
#include "state/State.h"

namespace state
{
	inline CardRef Cards::Manager::PushBack(Cards::Card&& card)
	{
		assert(card.GetZone() == kCardZoneInvalid); // Caller use manipulators to adjust. So we don't need to maintain any internal structure consistency
		CardRef ref = CardRef(cards_.PushBack(std::move(card)));
		return ref;
	}
}