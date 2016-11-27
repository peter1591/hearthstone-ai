#pragma once

#include "FlowControl/Manipulators/Manipulators.h"
#include "State/State.h"
#include "FlowControl/Result.h"
#include "FlowControl/Helpers/Utils.h"
#include "FlowControl/Helpers/ActionParameterWrapper.h"
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
			Attack(state::State & state, CardRef attacker, CardRef defender, ActionParameterGetter & action_parameters, RandomGenerator & random)
				: state_(state), attacker_(attacker), defender_(defender),
				  action_parameters_(action_parameters), random_(random)
			{

			}

			Result Go()
			{
				Result rc = kResultNotDetermined;

				CardRef origin_attacker = attacker_;
				CardRef origin_defender = defender_;

				do {
					if (!attacker_.IsValid()) return kResultNotDetermined;
					if (!defender_.IsValid()) return kResultNotDetermined;

					state_.event_mgr.TriggerEvent<state::Events::EventTypes::BeforeAttack>(state_, attacker_, defender_);
				} while (origin_attacker != attacker_ || origin_defender != defender_);

				state_.event_mgr.TriggerEvent<state::Events::EventTypes::OnAttack>(state_, attacker_, defender_);
				// TODO: attacker lose stealth

				Manipulators::StateManipulator(state_).Character(defender_).TakeDamage(
					state_.mgr.Get(attacker_).GetAttack());
				Manipulators::StateManipulator(state_).Character(attacker_).TakeDamage(
					state_.mgr.Get(defender_).GetAttack());

				state_.event_mgr.TriggerEvent<state::Events::EventTypes::AfterAttack>(state_, attacker_, defender_);

				{
					const state::Cards::Card & attacker_card = state_.mgr.Get(attacker_);
					if (attacker_card.GetCardType() == state::kCardTypeHero) {
						CardRef weapon_ref = attacker_card.GetRawData().weapon_ref;
						if (weapon_ref.IsValid()) {
							Manipulators::StateManipulator(state_).Weapon(weapon_ref).ReduceDurability(1);
						}
					}
				}

				if ((rc = Utils::CheckWinLoss(state_)) != kResultNotDetermined) return rc;

				// TODO: check deaths
				// TODO: check weapon deaths

				return kResultNotDetermined;
			}

		private:
			state::State & state_;
			CardRef attacker_;
			CardRef defender_;
			ActionParameterWrapper<ActionParameterGetter> action_parameters_;
			RandomGenerator & random_;
		};
	}
}