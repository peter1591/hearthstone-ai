#pragma once

#include "State/State.h"
#include "FlowControl/Result.h"
#include "FlowControl/Helpers/Utils.h"
#include "FlowControl/Helpers/ActionParameterWrapper.h"
#include "FlowControl/Helpers/DamageDealer.h"
#include "FlowControl/Helpers/EntityDeathHandler.h"
#include "FlowControl/Dispatchers/Minions.h"
#include "FlowControl/Context/BattleCry.h"
#include "FlowControl/Context/BeforeMinionSummoned.h"

namespace FlowControl
{
	namespace Helpers
	{
		template <class ActionParameterGetter, class RandomGenerator>
		class Attack
		{
		public:
			Attack(state::State & state, FlowContext & flow_context, state::CardRef attacker, state::CardRef defender, ActionParameterGetter & action_parameters, RandomGenerator & random)
				: state_(state), flow_context_(flow_context), attacker_(attacker), defender_(defender),
				  action_parameters_(action_parameters), random_(random)
			{

			}

			Result Go()
			{
				Result rc = kResultNotDetermined;

				DoAttack();

				if ((rc = EntityDeathHandler(state_, flow_context_).ProcessDeath()) != kResultNotDetermined) return rc;

				return rc;
			}

		private:
			void DoAttack()
			{
				while (true) {
					if (!attacker_.IsValid()) return;
					if (!defender_.IsValid()) return;

					CardRef origin_attacker = attacker_;
					CardRef origin_defender = defender_;
					state_.event_mgr.TriggerEvent<state::Events::EventTypes::BeforeAttack>(state_, attacker_, defender_);

					// TODO: check if it's mortally wounded
					// TODO: if still in play zone

					if (origin_attacker == attacker_ && origin_defender == defender_) break;
				}

				state_.event_mgr.TriggerEvent<state::Events::EventTypes::OnAttack>(state_, attacker_, defender_);
				// TODO: attacker lose stealth

				GetDamageDealer().DealDamage(defender_, state_.mgr.Get(attacker_).GetAttack());
				GetDamageDealer().DealDamage(attacker_, state_.mgr.Get(defender_).GetAttack());

				state_.event_mgr.TriggerEvent<state::Events::EventTypes::AfterAttack>(state_, attacker_, defender_);

				{
					const state::Cards::Card & attacker_card = state_.mgr.Get(attacker_);
					if (attacker_card.GetCardType() == state::kCardTypeHero) {
						CardRef weapon_ref = attacker_card.GetRawData().weapon_ref;
						if (weapon_ref.IsValid()) {
							GetDamageDealer().DealDamage(weapon_ref, 1);
						}
					}
				}
			}

		private:
			Helpers::DamageDealer GetDamageDealer() { return DamageDealer(state_, flow_context_); }

		private:
			state::State & state_;
			FlowContext & flow_context_;
			state::CardRef attacker_;
			state::CardRef defender_;
			ActionParameterWrapper<ActionParameterGetter> action_parameters_;
			RandomGenerator & random_;
		};
	}
}