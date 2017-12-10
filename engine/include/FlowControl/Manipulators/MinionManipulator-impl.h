#pragma once

#include "FlowControl/Manipulators/MinionManipulator.h"

namespace FlowControl
{
	namespace Manipulators
	{
		template <state::CardZone Zone>
		template <state::CardZone SwapWithZone>
		inline void MinionManipulator<Zone>::SwapWith(state::CardRef ref)
		{
			state_.GetZoneChanger<state::kCardTypeMinion, SwapWithZone>(ref)
				.template ChangeTo<state::kCardZoneSetASide>(state_.GetCard(ref).GetPlayerIdentifier());

			state_.GetZoneChanger<state::kCardTypeMinion, Zone>(card_ref_)
				.ReplaceBy(ref);

			FlowControl::Manipulate(state_, flow_context_).MinionInZone<state::kCardZoneSetASide>(card_ref_)
				.template MoveTo<SwapWithZone>(state_.GetCard(card_ref_).GetPlayerIdentifier());
		}

		template <state::CardZone Zone>
		template <state::CardZone ZoneTo>
		inline bool MinionManipulator<Zone>::PreMoveTo(state::PlayerIdentifier to_player)
		{
			(void)to_player; // only used in some template specialization

			if constexpr (ZoneTo == state::kCardZoneHand) {
				if (state_.GetBoard().Get(to_player).hand_.Full()) {
					Destroy();
					return false;
				}
			}

			if constexpr (ZoneTo == state::kCardZonePlay) {
				if (state_.GetBoard().Get(to_player).minions_.Full()) {
					Destroy();
					return false;
				}
			}

			if constexpr (Zone != ZoneTo) {
				state_.GetMutableCard(card_ref_).RestoreToDefault();
			}

			return true;
		}

		template <state::CardZone Zone>
		template <state::CardZone ZoneTo>
		inline void MinionManipulator<Zone>::MoveTo(state::PlayerIdentifier to_player)
		{
			if (!PreMoveTo<ZoneTo>(to_player)) return;

			state_.GetZoneChanger<state::kCardTypeMinion, Zone>(card_ref_)
				.template ChangeTo<ZoneTo>(to_player);
		}

		template <state::CardZone Zone>
		template <state::CardZone ZoneTo>
		inline void MinionManipulator<Zone>::MoveTo(state::PlayerIdentifier to_player, int pos)
		{
			if (!PreMoveTo<ZoneTo>(to_player)) return;

			state_.GetZoneChanger<state::kCardTypeMinion, Zone>(card_ref_)
				.template ChangeTo<ZoneTo>(to_player, pos);
		}
	}
}