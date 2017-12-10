#pragma once

#include "FlowControl/Manipulators/CardManipulator.h"

namespace FlowControl
{
	namespace Manipulators
	{
		class SecretManipulator : public CardManipulator
		{
		public:
			SecretManipulator(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref)
				: CardManipulator(state, flow_context, card_ref)
			{
				assert(GetCard().GetCardType() == state::kCardTypeSpell);
				assert(GetCard().IsSecretCard());
			}

			void Remove();

			void Reveal();
		};
	}
}