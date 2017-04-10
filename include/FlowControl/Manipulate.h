#pragma once

#include "state/State.h"
#include "state/Types.h"
#include "FlowControl/FlowContext.h"
#include "FlowControl/Manipulators/BoardManipulator.h"
#include "FlowControl/Manipulators/CardManipulator.h"
#include "FlowControl/Manipulators/CharacterManipulator.h"
#include "FlowControl/Manipulators/OnBoardMinionManipulator.h"
#include "FlowControl/Manipulators/MinionManipulator.h"
#include "FlowControl/Manipulators/HeroManipulator.h"
#include "FlowControl/Manipulators/HeroPowerManipulator.h"
#include "FlowControl/Manipulators/WeaponManipulator.h"
#include "FlowControl/Manipulators/SecretManipulator.h"

namespace FlowControl
{
	class Manipulate
	{
	public:
		Manipulate(state::State & state, FlowContext & flow_context);

		Manipulators::CardManipulator Card(state::CardRef ref);

		Manipulators::HeroManipulator CurrentHero();
		Manipulators::HeroManipulator OpponentHero();
		Manipulators::HeroManipulator Hero(state::PlayerIdentifier player);
		Manipulators::HeroManipulator Hero(state::CardRef hero_ref);

		Manipulators::HeroPowerManipulator HeroPower(state::PlayerIdentifier player);
		Manipulators::HeroPowerManipulator HeroPower(state::CardRef hero_power_ref);

		Manipulators::OnBoardMinionManipulator OnBoardMinion(state::CardRef ref) {
			return Manipulators::OnBoardMinionManipulator(state_, flow_context_, ref);
		}

		template <state::CardZone Zone>
		Manipulators::MinionManipulator<Zone> MinionInZone(state::CardRef ref) {
			return Manipulators::MinionManipulator<Zone>(state_, flow_context_, ref);
		}

		Manipulators::CharacterManipulator Character(state::CardRef ref);

		Manipulators::WeaponManipulator Weapon(state::CardRef ref);

		Manipulators::SecretManipulator Secret(state::CardRef ref);

		Manipulators::BoardManipulator Board();

	public: // bridge to flow context
		state::CardRef GetSpecifiedTarget() const { return flow_context_.GetSpecifiedTarget(); }
		state::IRandomGenerator & GetRandom() { return flow_context_.GetRandom();}

	public: // bridge to state::State
		state::Cards::Card const& GetCard(state::CardRef ref) { return state_.GetCard(ref); }
		state::aura::Manager & Aura() { return state_.GetAuraManager(); }

	public:
		state::CardRef GetRandomTarget(state::targetor::Targets const& target_info);

	private:
		state::State & state_;
		FlowControl::FlowContext & flow_context_;
	};
}