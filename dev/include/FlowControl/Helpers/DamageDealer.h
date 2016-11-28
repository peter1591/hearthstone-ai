#pragma once

#include "State/State.h"
#include "FlowControl/Manipulate.h"
#include "FlowControl/Context/OnTakeDamage.h"
#include "FlowControl/Helpers/EntityDeathHandler.h"

namespace FlowControl
{
	namespace Helpers
	{
		class DamageDealer
		{
		public:
			DamageDealer(state::State & state, Helpers::EntityDeathHandler & entity_death_handler) : 
				state_(state), entity_death_handler_(entity_death_handler)
			{
			}

			void DealDamage(state::CardRef target_ref, int origin_damage)
			{
				// Hooked events might change the damage amount, and/or the damage target
				Context::OnTakeDamage context(state_, target_ref, origin_damage);
				state_.event_mgr.TriggerEvent<state::Events::EventTypes::OnTakeDamage>(context);

				Manipulate(state_).Character(context.card_ref_).Damage().Take(context.damage_);

				entity_death_handler_.Add(context.card_ref_);
			}

		private:
			state::State & state_;
			Helpers::EntityDeathHandler & entity_death_handler_;
		};
	}
}