#pragma once

#include <utility>
#include "CloneableContainers/Vector.h"
#include "Entity/Card.h"
#include "EntitiesManager/CardRef.h"
#include "State/PlayerIdentifier.h"

namespace Manipulators
{
	class CharacterManipulator;
	class GeneralManipulator;
	class MinionManipulator;
	class SpellManipulator;

	class StateManipulator;
}

namespace State
{
	class State;
}

class EntitiesManager
{
	friend class Manipulators::StateManipulator;

public:
	typedef CloneableContainers::Vector<Entity::Card> ContainerType;

	const Entity::Card & Get(const CardRef & id) const
	{
		return cards_.Get(id.id);
	}

	template <typename T> CardRef PushBack(State::State & state, T&& card);

	Manipulators::MinionManipulator GetMinionManipulator(const CardRef & id);
	Manipulators::SpellManipulator GetSpellManipulator(const CardRef & id);

private:
	Entity::Card & GetMutable(const CardRef & id)
	{
		return cards_.Get(id.id);
	}

private:
	ContainerType cards_;
};
