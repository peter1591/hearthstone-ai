#pragma once

#include "State/Cards/Manager.h"

#include "Manipulators/StateManipulator.h"
#include "Manipulators/CharacterManipulator.h"
#include "Manipulators/MinionManipulator.h"
#include "State/State.h"

namespace State
{
	template <typename T>
	CardRef Cards::Manager::PushBack(State & state, T&& card)
	{
		CardRef ref = CardRef(cards_.PushBack(std::forward<T>(card)));
		Manipulators::StateManipulator(state).Card(ref).GetZoneChanger().Add(state);
		return ref;
	}
}