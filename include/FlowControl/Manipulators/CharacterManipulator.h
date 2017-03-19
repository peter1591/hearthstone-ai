#pragma once

#include "state/Types.h"
#include "state/Cards/Card.h"
#include "FlowControl/Manipulators/CardManipulator.h"
#include "FlowControl/Manipulators/Helpers/DamageHelper.h"

namespace FlowControl
{
	namespace Helpers { class DamageDealer; }

	namespace Manipulators
	{
		class CharacterManipulator : public CardManipulator
		{
		public:
			CharacterManipulator(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card)
				: CardManipulator(state, flow_context, card_ref, card)
			{
				assert(card.GetCardType() == state::kCardTypeMinion ||
					card.GetCardType() == state::kCardTypeHero);
			}

			void Taunt(bool v) { card_.SetTaunt(v); }
			void Shield(bool v) { card_.SetShield(v); }
			void Charge(bool v) { card_.SetCharge(v); }

			void AfterAttack()
			{
				card_.IncreaseNumAttacksThisTurn();
			}
		};
	}
}