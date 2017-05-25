#pragma once

#include "FlowControl/Manipulators/CharacterManipulator.h"

namespace FlowControl
{
	namespace Manipulators
	{
		template <state::CardZone Zone>
		class MinionManipulator : public CharacterManipulator
		{
		public:
			MinionManipulator(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref)
				: CharacterManipulator(state, flow_context, card_ref)
			{
				assert(GetCard().GetCardType() == state::kCardTypeMinion);
				assert(GetCard().GetZone() == Zone);
			}

			void Destroy() {
				if (Zone == state::kCardZonePlay) { // TODO: if-constexpr
					GetCard().SetPendingDestroy();
					flow_context_.AddDeadEntryHint(state_, card_ref_);
				}
			}

			template<state::CardZone SwapWithZone>
			void SwapWith(state::CardRef ref);

			template <state::CardZone ZoneTo> void MoveTo(state::PlayerIdentifier to_player);
			template <state::CardZone ZoneTo> void MoveTo(state::PlayerIdentifier to_player, int pos);

			template <state::CardZone ZoneTo> void MoveTo() {
				return MoveTo<ZoneTo>(GetCard().GetPlayerIdentifier());
			}

		private:
			template <state::CardZone ZoneTo>
			bool PreMoveTo(state::PlayerIdentifier to_player);
		};
	}
}