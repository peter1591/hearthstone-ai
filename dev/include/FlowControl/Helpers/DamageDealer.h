#pragma once

#include "State/State.h"
#include "FlowControl/Manipulate.h"
#include "FlowControl/Context/OnTakeDamage.h"

namespace FlowControl
{
	namespace Helpers
	{
		class DamageDealer
		{
		public:
			DamageDealer(state::State & state) : state_(state) {}

			void DealDamage(state::CardRef target_ref, int origin_damage)
			{
				// Hooked events might change the damage amount, and/or the damage target
				Context::OnTakeDamage context(state_, target_ref, origin_damage);
				state_.event_mgr.TriggerEvent<state::Events::EventTypes::OnTakeDamage>(context);

				Manipulate(state_).Character(context.card_ref_).Damage().Take(context.damage_);
			}

		private:
			state::State & state_;
		};
	}
}