#pragma once

#include "FlowControl/Manipulators/CharacterManipulator.h"

namespace FlowControl
{
	namespace Manipulators
	{
		class HeroManipulator : public CharacterManipulator
		{
		public:
			HeroManipulator(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card)
				: CharacterManipulator(state, flow_context, card_ref, card)
			{
			}
		};
	}
}