#pragma once

#include "state/State.h"
#include "state/targetor/Targets-impl.h"
#include "state/detail/InvokeCallback-impl.h"

#include "engine/FlowControl/FlowContext.h"
#include "engine/FlowControl/FlowContext-impl.h"
#include "engine/FlowControl/Manipulate.h"

#include "engine/FlowControl/aura/Handler-impl.h"
#include "engine/FlowControl/onplay/Handler-impl.h"
#include "engine/FlowControl/enchantment/Handler-impl.h"
#include "engine/FlowControl/Manipulators/PlayerManipulator-impl.h"
#include "engine/FlowControl/Manipulators/HeroManipulator-impl.h"
#include "engine/FlowControl/Manipulators/HeroPowerManipulator-impl.h"
#include "engine/FlowControl/Manipulators/BoardManipulator-impl.h"
#include "engine/FlowControl/Manipulators/CardManipulator-impl.h"
#include "engine/FlowControl/Manipulators/OnBoardMinionManipulator-impl.h"
#include "engine/FlowControl/Manipulators/MinionManipulator-impl.h"
#include "engine/FlowControl/Manipulators/SecretManipulator-impl.h"
#include "engine/FlowControl/Manipulators/Helpers/EnchantmentHelper-impl.h"
#include "engine/FlowControl/Manipulators/Helpers/DamageHelper-impl.h"

#include "engine/FlowControl/detail/Resolver.h"

namespace engine {
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
			engine::Result PerformOperation();

		private:
			engine::Result PlayCard(int hand_idx);
			engine::Result Attack(state::CardRef attacker);
			engine::Result HeroPower();
			engine::Result EndTurn();

		private:
			state::CardRef GetDefender(state::CardRef attacker);

		private:
			engine::Result Resolve();

			void PlayCardInternal(state::CardRef card_ref);
			void CostCrystal(int amount);
			void CostHealth(int amount);

			template <state::CardType> bool PlayCardPhase(state::CardRef card_ref);
			template <state::CardType> bool PlayCardPhaseInternal(state::CardRef card_ref);

			bool PlayMinionCardPhase(state::CardRef card_ref);
			bool PlayWeaponCardPhase(state::CardRef card_ref);
			bool PlayHeroPowerCardPhase(state::CardRef card_ref);
			bool PlaySpellCardPhase(state::CardRef card_ref);

			void AttackInternal(state::CardRef attacker);
			bool AttackPhase(state::CardRef attacker);

			void EndTurnInternal();
			void EndTurnPhase();
			void StartTurnPhase();
			void DrawCardPhase();

			void HeroPowerInternal();
			bool HeroPowerPhase();
			int GetMaxHeroPowerUseThisTurn();

			bool SetResult(engine::Result result);
			bool SetInvalid() {
				return SetResult(engine::Result::kResultInvalid);
			}

		private:
			state::State & state_;
			FlowContext & flow_context_;
		};
	}
}