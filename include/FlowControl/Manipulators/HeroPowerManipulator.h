#pragma once

#include "state/Cards/Card.h"
#include "FlowControl/Manipulators/CardManipulator.h"

namespace FlowControl
{
	namespace Manipulators
	{
		class HeroPowerManipulator : public CardManipulator
		{
		public:
			HeroPowerManipulator(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card &card)
				: CardManipulator(state, flow_context, card_ref, card)
			{
				assert(card.GetCardType() == state::kCardTypeHeroPower);
			}

			void TurnStart()
			{
				card_.ClearUsedThisTurn();
			}

			void IncreaseUsedThisTurn()
			{
				card_.IncreaseUsedThisTurn();
			}
		};
	}
}