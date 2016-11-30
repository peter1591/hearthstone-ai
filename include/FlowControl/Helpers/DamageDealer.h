#pragma once

#include "State/State.h"
#include "FlowControl/Manipulate.h"
#include "FlowControl/Context/OnTakeDamage.h"

namespace FlowControl
{
	namespace Helpers
	{
		class DamageDealer // TODO: move to manipulators/helpers/
		{
		public:
			DamageDealer(state::State & state, FlowContext & flow_context) : 
				state_(state), flow_context_(flow_context)
			{
			}

			void DealDamage(state::CardRef target_ref, int origin_damage)
			{
				// Hooked events might change the damage amount, and/or the damage target
				Context::OnTakeDamage context(state_, target_ref, origin_damage);
				state_.event_mgr.TriggerEvent<state::Events::EventTypes::OnTakeDamage>(context);

				Manipulate(state_, flow_context_).Character(context.card_ref_).Damage().Take(context.damage_);

				flow_context_.AddDeadEntryHint(state_, context.card_ref_);
			}

		private:
			state::State & state_;
			FlowContext & flow_context_;
		};
	}
}