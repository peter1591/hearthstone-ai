#pragma once

#include "state/Types.h"
#include "FlowControl/Manipulators/BoardManipulator.h"
#include "FlowControl/Manipulators/CardManipulator.h"
#include "FlowControl/Manipulators/CharacterManipulator.h"
#include "FlowControl/Manipulators/MinionManipulator.h"
#include "FlowControl/Manipulators/HeroManipulator.h"
#include "FlowControl/Manipulators/WeaponManipulator.h"

namespace state { class State; }

namespace FlowControl
{
	class FlowContext;

	class Manipulate
	{
	public:
		Manipulate(state::State & state, FlowContext & flow_context) : state_(state), flow_context_(flow_context) { }

		Manipulators::CardManipulator Card(state::CardRef ref);
		Manipulators::HeroManipulator CurrentHero();
		Manipulators::HeroManipulator OpponentHero();
		Manipulators::HeroManipulator Hero(state::PlayerIdentifier player);
		Manipulators::MinionManipulator Minion(state::CardRef ref);
		Manipulators::CharacterManipulator Character(state::CardRef ref);
		Manipulators::WeaponManipulator Weapon(state::CardRef ref);
		Manipulators::BoardManipulator Board();

	private:
		state::State & state_;
		FlowContext & flow_context_;
	};
}