#pragma once

#include "State/Cards/Manager.h"

#include "FlowControl/Manipulate.h"
#include "FlowControl/Manipulators/CharacterManipulator.h"
#include "FlowControl/Manipulators/MinionManipulator.h"
#include "State/State.h"

namespace state
{
	template <typename T>
	CardRef Cards::Manager::PushBack(State & state, FlowControl::FlowContext & flow_context, T&& card)
	{
		CardRef ref = CardRef(cards_.PushBack(std::forward<T>(card)));

		FlowControl::Manipulate(state, flow_context).Card(ref).Zone().Add();
		return ref;
	}
}