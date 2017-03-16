#pragma once

#include "state/State.h"
#include "state/targetor/Targets-impl.h"

#include "FlowControl/ActionParameterWrapper.h"
#include "FlowControl/FlowContext.h"
#include "FlowControl/FlowContext-impl.h"
#include "FlowControl/Contexts.h"
#include "FlowControl/Contexts-impl.h"
#include "FlowControl/Helpers/Resolver.h"
#include "FlowControl/Manipulate.h"

#include "FlowControl/aura/Handler-impl.h"
#include "FlowControl/enchantment/Handler-impl.h"
#include "FlowControl/Manipulators/HeroManipulator-impl.h"
#include "FlowControl/Manipulators/BoardManipulator-impl.h"
#include "FlowControl/Manipulators/CardManipulator-impl.h"
#include "FlowControl/Manipulators/MinionManipulator-impl.h"
#include "FlowControl/Manipulators/Helpers/EnchantmentHelper-impl.h"
#include "FlowControl/Manipulators/Helpers/DamageHelper-impl.h"
#include "FlowControl/Manipulators/Helpers/HealHelper-impl.h"

#include "FlowControl/Dispatchers/Minions.h"
#include "FlowControl/Dispatchers/Weapons.h"
#include "Cards/Cards.h"

namespace FlowControl
{
	class FlowController
	{
	public:
		FlowController(state::State & state, FlowControl::IActionParameterGetter & action_parameters, state::IRandomGenerator & random)
			: state_(state), action_parameters_(action_parameters), random_(random),
			flow_context_(random_, action_parameters_)
		{
		}

	public: // main flow
		Result PlayCard(int hand_idx);
		Result EndTurn();
		Result Attack(state::CardRef attacker, state::CardRef defender);
		Result Resolve();

	public: // might be called in card client code

	private:
		void PlayCardInternal(int hand_idx);
		bool PlayCardPhase(int hand_idx);
		bool PlayMinionCardPhase(int hand_idx, state::CardRef card_ref, state::Cards::Card const& card);
		bool PlayWeaponCardPhase(int hand_idx, state::CardRef card_ref, state::Cards::Card const& card);
		
		void AttackInternal(state::CardRef attacker, state::CardRef defender);
		bool AttackPhase(state::CardRef attacker, state::CardRef defender);
		bool IsAttackable(state::CardRef attacker);
		int GetAttackValue(state::CardRef ref);

		void EndTurnInternal();
		void EndTurnPhase();
		void StartTurnPhase();
		void DrawCardPhase();

		int CalculateSpellDamage(state::board::Player const& player);
		bool SetResult(Result result);

	public:
		state::State & state_;
		state::IRandomGenerator & random_;
		FlowControl::ActionParameterWrapper action_parameters_;
		FlowContext flow_context_;
	};
}