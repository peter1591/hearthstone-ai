#pragma once

#include "state/Cards/Card.h"
#include "FlowControl/Manipulators/MinionManipulator.h"
#include "Cards/id-map.h"

namespace FlowControl
{
	namespace Manipulators
	{
		class OnBoardMinionManipulator : public MinionManipulator<state::kCardZonePlay>
		{
		public:
			OnBoardMinionManipulator(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref)
				: MinionManipulator(state, flow_context, card_ref)
			{
				assert(GetCard().GetCardType() == state::kCardTypeMinion);
				assert(GetCard().GetZone() == state::kCardZonePlay);
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

			void ChangeOwner(state::PlayerIdentifier new_owner);

			state::CardRef Transform(Cards::CardId card_id);

			state::CardRef BecomeCopyof(state::CardRef card_ref);
			state::CardRef BecomeCopyof(state::Cards::Card const& card);
		};
	}
}