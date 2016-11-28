#pragma once

#include "State/State.h"

namespace FlowControl
{
	namespace Helpers { class DamageDealer; }

	namespace Manipulators
	{
		namespace Helpers
		{
			class TakeDamageHelper
			{
				friend class FlowControl::Helpers::DamageDealer;

			public:
				TakeDamageHelper(state::Cards::Card & card) : card_(card) {}

			private:
				void Take(int damage) { card_.GetDamageSetter().Set(card_.GetDamage() + damage); }

			private:
				state::Cards::Card & card_;
			};
		}
	}
}