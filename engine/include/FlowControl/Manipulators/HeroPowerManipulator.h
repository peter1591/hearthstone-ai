#pragma once

#include "state/Cards/Card.h"
#include "FlowControl/Manipulators/CardManipulator.h"
#include "Cards/id-map.h"

namespace FlowControl
{
	namespace Manipulators
	{
		class HeroPowerManipulator : public CardManipulator
		{
		public:
			HeroPowerManipulator(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref)
				: CardManipulator(state, flow_context, card_ref)
			{
				assert(GetCard().GetCardType() == state::kCardTypeHeroPower);
			}

			void TurnStart()
			{
				GetCard().ClearUsedThisTurn();
				GetCard().SetUsable();
			}

			void IncreaseUsedThisTurn() { GetCard().IncreaseUsedThisTurn(); }
			void SetUsable() { GetCard().SetUsable(true); }
			void SetUnusable() { GetCard().SetUsable(false); }
		};
	}
}