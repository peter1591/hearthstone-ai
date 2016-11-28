#pragma once

#include "State/Types.h"
#include "State/State.h"
#include "State/Cards/Card.h"
#include "State/Cards/Manager.h"
#include "FlowControl/Manipulators/CardManipulator.h"

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

		class CharacterManipulator : public CardManipulator
		{
		public:
			CharacterManipulator(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card)
				: CardManipulator(state, flow_context, card_ref, card)
			{
			}

			Helpers::TakeDamageHelper Damage() { return Helpers::TakeDamageHelper(card_); }
		};
	}
}