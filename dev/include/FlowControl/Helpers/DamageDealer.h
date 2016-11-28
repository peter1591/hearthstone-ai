#pragma once

#include "State/State.h"
#include "FlowControl/Manipulate.h"

namespace FlowControl
{
	namespace Helpers
	{
		class DamageDealer
		{
		public:
			DamageDealer(state::State & state) : state_(state) {}

			void DealDamage(state::CardRef target_ref, int damage)
			{
				// TODO: trigger events

				Manipulate(state_).Character(target_ref).Damage().Take(damage);
			}

		private:
			state::State & state_;
		};
	}
}