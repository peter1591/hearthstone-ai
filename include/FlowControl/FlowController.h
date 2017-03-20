#pragma once

#include "state/State.h"
#include "state/targetor/Targets-impl.h"

#include "FlowControl/FlowContext.h"
#include "FlowControl/FlowContext-impl.h"
#include "FlowControl/Manipulate.h"

#include "FlowControl/aura/Handler-impl.h"
#include "FlowControl/flag_aura/Handler-impl.h"
#include "FlowControl/onplay/Handler-impl.h"
#include "FlowControl/enchantment/Handler-impl.h"
#include "FlowControl/Manipulators/HeroManipulator-impl.h"
#include "FlowControl/Manipulators/BoardManipulator-impl.h"
#include "FlowControl/Manipulators/CardManipulator-impl.h"
#include "FlowControl/Manipulators/MinionManipulator-impl.h"
#include "FlowControl/Manipulators/Helpers/EnchantmentHelper-impl.h"
#include "FlowControl/Manipulators/Helpers/DamageHelper-impl.h"

#include "FlowControl/detail/Resolver.h"
#include "Cards/Cards.h"

namespace FlowControl
{
	class FlowController
	{
	public:
		FlowController(state::State & state, FlowContext & flow_context)
			: state_(state), flow_context_(flow_context)
		{
		}

	public: // main flow
		Result PlayCard(int hand_idx);
		Result EndTurn();
		Result Attack(state::CardRef attacker, state::CardRef defender);
		Result HeroPower();
		Result Resolve();

	private:
		void PlayCardInternal(int hand_idx);

		template <state::CardType> bool PlayCardPhase(state::CardRef card_ref, state::Cards::Card const& card);
		template <state::CardType> bool PlayCardPhaseInternal(state::CardRef card_ref, state::Cards::Card const& card);

		bool PlayMinionCardPhase(state::CardRef card_ref, state::Cards::Card const& card);
		bool PlayWeaponCardPhase(state::CardRef card_ref, state::Cards::Card const& card);
		bool PlayHeroPowerCardPhase(state::CardRef card_ref, state::Cards::Card const& card);
		bool PlaySpellCardPhase(state::CardRef card_ref, state::Cards::Card const& card);
		
		void AttackInternal(state::CardRef attacker, state::CardRef defender);
		bool AttackPhase(state::CardRef attacker, state::CardRef defender);
		bool IsAttackable(state::CardRef attacker);
		int GetAttackValue(state::CardRef ref);

		void EndTurnInternal();
		void EndTurnPhase();
		void StartTurnPhase();
		void DrawCardPhase();

		void HeroPowerInternal();
		bool HeroPowerPhase();
		int GetMaxHeroPowerUseThisTurn();

		bool SetResult(Result result);

	private:
		state::State & state_;
		FlowContext & flow_context_;
	};
}