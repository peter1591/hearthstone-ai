#pragma once

#include "state/Cards/Card.h"
#include "FlowControl/Manipulators/CharacterManipulator.h"
#include "Cards/id-map.h"

namespace FlowControl
{
	namespace Manipulators
	{
		class MinionManipulator : public CharacterManipulator
		{
		public:
			MinionManipulator(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card &card)
				: CharacterManipulator(state, flow_context, card_ref, card)
			{
				assert(card.GetCardType() == state::kCardTypeMinion);
			}

			void AfterSummoned()
			{
				card_.SetJustPlayedFlag(true);
			}

			void TurnStart()
			{
				card_.SetJustPlayedFlag(false);
				card_.ClearNumAttacksThisTurn();
			}

			void Silence();

			void Transform(Cards::CardId card_id);
		};
	}
}