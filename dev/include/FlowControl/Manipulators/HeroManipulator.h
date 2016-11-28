#pragma once

#include "FlowControl/Manipulators/CharacterManipulator.h"

namespace FlowControl
{
	namespace Manipulators
	{
		class HeroManipulator : public CharacterManipulator
		{
		public:
			HeroManipulator(state::State & state, state::CardRef card_ref, state::Cards::Card & card)
				: CharacterManipulator(state, card_ref, card)
			{
			}
		};
	}
}