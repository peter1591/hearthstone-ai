#pragma once

#include "EntitiesManager/EntitiesManager.h"

#include "Manipulators/StateManipulator.h"
#include "Manipulators/CharacterManipulator.h"
#include "Manipulators/MinionManipulator.h"

namespace State { class State; }

template <typename T>
CardRef EntitiesManager::PushBack(State::State & state, T&& card)
{
	CardRef ref = CardRef(cards_.PushBack(std::forward<T>(card)));
	Manipulators::StateManipulator(state).Card(ref).GetZoneChanger().Add(state);
	return ref;
}