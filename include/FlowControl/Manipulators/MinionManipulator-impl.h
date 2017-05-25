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
			state_.GetZoneChanger<state::kCardTypeMinion, SwapWithZone>(
				FlowControl::Manipulate(state_, flow_context_), ref
				).ChangeTo<state::kCardZoneSetASide>(state_.GetCard(ref).GetPlayerIdentifier());

			state_.GetZoneChanger<state::kCardTypeMinion, Zone>(
				FlowControl::Manipulate(state_, flow_context_), card_ref_
				).ReplaceBy(ref);

			FlowControl::Manipulate(state_, flow_context_).MinionInZone<state::kCardZoneSetASide>(card_ref_)
				.MoveTo<SwapWithZone>(state_.GetCard(card_ref_).GetPlayerIdentifier());
		}

		template <state::CardZone Zone>
		template <state::CardZone ZoneTo>
		inline bool MinionManipulator<Zone>::PreMoveTo(state::PlayerIdentifier to_player)
		{
			if (ZoneTo == state::kCardZoneHand) { // TODO: use if-constexpr when compiler supports
				if (state_.GetBoard().Get(to_player).hand_.Full()) {
					Destroy();
					return false;
				}
			}

			if (ZoneTo == state::kCardZonePlay) { // TODO: use if-constexpr when compiler supports
				if (state_.GetBoard().Get(to_player).minions_.Full()) {
					Destroy();
					return false;
				}
			}

			if (Zone != ZoneTo) { // TODO: use if-constexpr
				state_.GetMutableCard(card_ref_).RestoreToDefault();
			}

			return true;
		}

		template <state::CardZone Zone>
		template <state::CardZone ZoneTo>
		inline void MinionManipulator<Zone>::MoveTo(state::PlayerIdentifier to_player)
		{
			if (!PreMoveTo<ZoneTo>(to_player)) return;

			state_.GetZoneChanger<state::kCardTypeMinion, Zone>(
				FlowControl::Manipulate(state_, flow_context_), card_ref_
				).ChangeTo<ZoneTo>(to_player);
		}

		template <state::CardZone Zone>
		template <state::CardZone ZoneTo>
		inline void MinionManipulator<Zone>::MoveTo(state::PlayerIdentifier to_player, int pos)
		{
			if (!PreMoveTo<ZoneTo>(to_player)) return;

			state_.GetZoneChanger<state::kCardTypeMinion, Zone>(
				FlowControl::Manipulate(state_, flow_context_), card_ref_
				).ChangeTo<ZoneTo>(to_player, pos);
		}
	}
}