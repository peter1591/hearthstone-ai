#pragma once

#include "FlowControl/Manipulators/CardManipulator.h"

namespace FlowControl
{
	namespace Manipulators
	{
		class SecretManipulator : public CardManipulator
		{
		public:
			SecretManipulator(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card &card)
				: CardManipulator(state, flow_context, card_ref, card)
			{
				assert(card.GetCardType() == state::kCardTypeSecret);
			}

			void Remove()
			{
				assert(card_.GetZone() == state::kCardZonePlay);
				state_.GetZoneChanger<state::kCardZonePlay, state::kCardTypeSecret>(flow_context_.GetRandom(), card_ref_)
					.ChangeTo<state::kCardZoneGraveyard>(card_.GetPlayerIdentifier());
			}
		};
	}
}