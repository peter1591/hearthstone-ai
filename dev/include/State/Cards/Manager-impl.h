#pragma once

#include "State/Cards/Manager.h"

#include "FlowControl/Manipulators/StateManipulator.h"
#include "FlowControl/Manipulators/CharacterManipulator.h"
#include "FlowControl/Manipulators/MinionManipulator.h"
#include "State/State.h"

namespace state
{
	template <typename T>
	CardRef Cards::Manager::PushBack(State & state, T&& card)
	{
		CardRef ref = CardRef(cards_.PushBack(std::forward<T>(card)));
		FlowControl::Manipulators::StateManipulator(state).Card(ref).Zone().Add();
		return ref;
	}
}