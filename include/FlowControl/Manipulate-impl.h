#pragma once
#include "FlowControl/Manipulate.h"

#include "state/State.h"
#include "FlowControl/FlowContext.h"

namespace FlowControl
{
	inline Manipulate::Manipulate(state::State & state, FlowControl::FlowContext & flow_context)
		: state_(state), flow_context_(flow_context)
	{
	}

	inline Manipulators::CardManipulator Manipulate::Card(state::CardRef ref) const
	{
		return Manipulators::CardManipulator(state_, flow_context_, ref);
	}

	inline Manipulators::HeroManipulator Manipulate::CurrentHero() const
	{
		return Hero(state_.GetCurrentPlayerId());
	}

	inline Manipulators::PlayerManipulator Manipulate::CurrentPlayer() const
	{
		return Player(state_.GetCurrentPlayerId());
	}
	
	inline Manipulators::PlayerManipulator Manipulate::Player(state::PlayerIdentifier player) const
	{
		return Manipulators::PlayerManipulator(state_, flow_context_, player);
	}

	inline Manipulators::HeroManipulator Manipulate::OpponentHero() const
	{
		return Hero(state_.GetCurrentPlayerId().Opposite());
	}

	inline Manipulators::HeroManipulator Manipulate::Hero(state::PlayerIdentifier player) const
	{
		state::CardRef ref = state_.GetBoard().Get(player).GetHeroRef();
		assert(state_.GetCardsManager().Get(ref).GetPlayerIdentifier() == player);
		return Manipulators::HeroManipulator(state_, flow_context_, ref);
	}

	inline Manipulators::HeroManipulator Manipulate::Hero(state::CardRef hero_ref) const
	{
		return Manipulators::HeroManipulator(state_, flow_context_, hero_ref);
	}

	inline Manipulators::HeroPowerManipulator Manipulate::HeroPower(state::CardRef hero_power_ref) const
	{
		assert(hero_power_ref.IsValid());
		return Manipulators::HeroPowerManipulator(state_, flow_context_, hero_power_ref);
	}

	inline Manipulators::HeroPowerManipulator Manipulate::HeroPower(state::PlayerIdentifier player) const
	{
		return HeroPower(state_.GetBoard().Get(player).GetHeroPowerRef());
	}

	inline Manipulators::WeaponManipulator Manipulate::Weapon(state::PlayerIdentifier player) const
	{
		state::CardRef weapon_ref = state_.GetBoard().Get(player).GetWeaponRef();
		return Weapon(weapon_ref);
	}
	inline Manipulators::WeaponManipulator Manipulate::Weapon(state::CardRef ref) const
	{
		return Manipulators::WeaponManipulator(state_, flow_context_, ref);
	}

	inline Manipulators::SecretManipulator Manipulate::OnBoardSecret(state::CardRef ref) const
	{
		return Manipulators::SecretManipulator(state_, flow_context_, ref);
	}

	inline Manipulators::BoardManipulator Manipulate::Board() const
	{
		return Manipulators::BoardManipulator(state_, flow_context_);
	}

	inline state::CardRef Manipulate::GetRandomTarget(state::targetor::Targets const & target_info) const
	{
		std::vector<state::CardRef> targets;
		target_info.Fill(state_, targets);

		size_t count = targets.size();
		if (count == 0) return state::CardRef();
		if (count == 1) return targets.front();

		return targets[flow_context_.GetRandom().Get(count)];
	}

	inline state::Cards::Card const& Manipulate::GetCard(state::CardRef ref) const
	{
		return state_.GetCard(ref);
	}
	
	inline int Manipulate::GetCardAttackConsiderWeapon(state::CardRef ref) const
	{
		return state_.GetCardAttackConsiderWeapon(ref);
	}
	
	inline state::aura::Manager & Manipulate::Aura() const
	{
		return state_.GetAuraManager();
	}
}