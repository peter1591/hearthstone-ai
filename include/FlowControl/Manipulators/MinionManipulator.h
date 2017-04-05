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
			MinionManipulator(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref)
				: CharacterManipulator(state, flow_context, card_ref)
			{
				assert(GetCard().GetCardType() == state::kCardTypeMinion);
			}

			void AfterSummoned()
			{
			}

			void TurnStart()
			{
				GetCard().SetJustPlayedFlag(false);
				GetCard().ClearNumAttacksThisTurn();
			}

			void Silence();

			void Destroy() {
				GetCard().SetPendingDestroy();
				flow_context_.AddDeadEntryHint(state_, card_ref_);
			}

			state::CardRef Transform(Cards::CardId card_id);

			state::CardRef BecomeCopyof(state::CardRef card_ref);
			state::CardRef BecomeCopyof(state::Cards::Card const& card);

			template <state::CardZone Zone>
			void MoveToHand(state::PlayerIdentifier player);

			template <state::CardZone ZoneFrom, state::CardZone ZoneTo>
			void MoveTo(state::PlayerIdentifier to_player);
		};
	}
}