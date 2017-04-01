#pragma once

#include "state/State.h"
#include "state/targetor/Targets-impl.h"
#include "state/detail/InvokeCallback-impl.h"

#include "FlowControl/FlowContext.h"
#include "FlowControl/FlowContext-impl.h"
#include "FlowControl/Manipulate.h"

#include "FlowControl/aura/Handler-impl.h"
#include "FlowControl/flag_aura/Handler-impl.h"
#include "FlowControl/onplay/Handler-impl.h"
#include "FlowControl/enchantment/Handler-impl.h"
#include "FlowControl/Manipulators/HeroManipulator-impl.h"
#include "FlowControl/Manipulators/HeroPowerManipulator-impl.h"
#include "FlowControl/Manipulators/BoardManipulator-impl.h"
#include "FlowControl/Manipulators/CardManipulator-impl.h"
#include "FlowControl/Manipulators/MinionManipulator-impl.h"
#include "FlowControl/Manipulators/SecretManipulator-impl.h"
#include "FlowControl/Manipulators/Helpers/EnchantmentHelper-impl.h"
#include "FlowControl/Manipulators/Helpers/DamageHelper-impl.h"

#include "FlowControl/detail/Resolver.h"

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

	private:
		Result Resolve();

		void PlayCardInternal(int hand_idx);
		bool CostCrystal(int amount);
		bool CostHealth(int amount);

		template <state::CardType> bool PlayCardPhase(state::CardRef card_ref);
		template <state::CardType> bool PlayCardPhaseInternal(state::CardRef card_ref);

		bool PlayMinionCardPhase(state::CardRef card_ref);
		bool PlayWeaponCardPhase(state::CardRef card_ref);
		bool PlayHeroPowerCardPhase(state::CardRef card_ref);
		bool PlaySpellCardPhase(state::CardRef card_ref);
		bool PlaySecretCardPhase(state::CardRef card_ref);
		
		void AttackInternal(state::CardRef attacker, state::CardRef defender);
		bool AttackPhase(state::CardRef attacker, state::CardRef defender);
		bool IsAttackable(state::CardRef attacker);
		bool IsDefendable(state::CardRef defender);
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