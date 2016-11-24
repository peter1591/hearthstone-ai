#pragma once

#include "State/Cards/Manager.h"

#include "State/Manipulators/StateManipulator.h"
#include "State/Manipulators/CharacterManipulator.h"
#include "State/Manipulators/MinionManipulator.h"
#include "State/State.h"

namespace State
{
	template <typename T>
	CardRef Cards::Manager::PushBack(State & state, T&& card)
	{
		CardRef ref = CardRef(cards_.PushBack(std::forward<T>(card)));
		Manipulators::StateManipulator(state).Card(ref).Zone().Add();
		return ref;
	}
}