#pragma once

#include "state/Cards/Card.h"
#include "FlowControl/Manipulators/MinionManipulator.h"
#include "Cards/id-map.h"

namespace FlowControl
{
	namespace Manipulators
	{
		// The minion is expected to be on the play zone
		// If it's not, all operations will be no-ops.
		class OnBoardMinionManipulator : public CharacterManipulator
		{
		public:
			OnBoardMinionManipulator(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref)
				: CharacterManipulator(state, flow_context, card_ref)
			{
				assert(GetCard().GetCardType() == state::kCardTypeMinion);
			}

			void AfterSummoned()
			{
			}

			void TurnStart()
			{
				if (GetCard().GetZone() != state::kCardZonePlay) return;
				GetCard().SetJustPlayedFlag(false);
				GetCard().ClearNumAttacksThisTurn();
			}

			void Silence();
			void Destroy() {
				if(GetCard().GetZone() != state::kCardZonePlay) return;
				return MinionManipulator<state::kCardZonePlay>(state_, flow_context_, card_ref_)
					.Destroy();
			}

			void ChangeOwner(state::PlayerIdentifier new_owner);

			state::CardRef Transform(Cards::CardId card_id);

			state::CardRef BecomeCopyof(state::CardRef card_ref);
			state::CardRef BecomeCopyof(state::Cards::Card const& card);

			bool Alive() {
				if (GetCard().GetZone() != state::kCardZonePlay) return false;
				if (GetCard().GetPendingDestroy()) return false;
				if (GetCard().GetHP() <= 0) return false;
				return true;
			}

			void AddDeathrattle(FlowControl::deathrattle::Handler::DeathrattleCallback* deathrattle) {
				if (GetCard().GetZone() != state::kCardZonePlay) return;
				state_.GetMutableCard(card_ref_).GetMutableDeathrattleHandler().Add(deathrattle);
			}

		public: // bridge to MinionManipulator<state::kCardZonePlay>
			template<state::CardZone SwapWithZone> void SwapWith(state::CardRef ref) {
				if (GetCard().GetZone() != state::kCardZonePlay) return;
				return MinionManipulator<state::kCardZonePlay>(state_, flow_context_, card_ref_).SwapWith<SwapWithZone>(ref);
			}

			template <state::CardZone ZoneTo> void MoveTo(state::PlayerIdentifier to_player) {
				if (GetCard().GetZone() != state::kCardZonePlay) return;
				return MinionManipulator<state::kCardZonePlay>(state_, flow_context_, card_ref_)
					.MoveTo<ZoneTo>(to_player);
			}

			template <state::CardZone ZoneTo> void MoveTo() {
				return MoveTo<ZoneTo>(GetCard().GetPlayerIdentifier());
			}
		};
	}
}