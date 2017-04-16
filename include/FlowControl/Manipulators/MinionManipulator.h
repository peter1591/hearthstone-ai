#pragma once

#include "FlowControl/Manipulators/CharacterManipulator.h"

namespace FlowControl
{
	namespace Manipulators
	{
		template <state::CardZone Zone>
		class MinionManipulator : public CharacterManipulator<Zone>
		{
		public:
			MinionManipulator(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref)
				: CharacterManipulator(state, flow_context, card_ref)
			{
				assert(GetCard().GetCardType() == state::kCardTypeMinion);
				assert(GetCard().GetZone() == Zone);
			}

			template<state::CardZone SwapWithZone>
			void SwapWith(state::CardRef ref);

			template <state::CardZone ZoneTo> void MoveTo(state::PlayerIdentifier to_player);

			template <state::CardZone ZoneTo> void MoveTo() {
				return MoveTo<ZoneTo>(GetCard().GetPlayerIdentifier());
			}
		};
	}
}