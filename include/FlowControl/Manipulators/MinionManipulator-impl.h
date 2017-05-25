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
		inline void MinionManipulator<Zone>::MoveTo(state::PlayerIdentifier to_player)
		{
			static_assert(Zone != ZoneTo, "Zone should actually changed. If in the same zone, call ChangeOwner() instead.");

			if (ZoneTo == state::kCardZoneHand) { // TODO: use if-constexpr when compiler supports
				if (state_.GetBoard().Get(to_player).hand_.Full()) {
					return Destroy();
				}
			}

			if (ZoneTo == state::kCardZonePlay) { // TODO: use if-constexpr when compiler supports
				if (state_.GetBoard().Get(to_player).minions_.Full()) {
					return Destroy();
				}
			}

			state_.GetZoneChanger<state::kCardTypeMinion, Zone>(
				FlowControl::Manipulate(state_, flow_context_), card_ref_
				).ChangeTo<ZoneTo>(to_player);

			state_.GetMutableCard(card_ref_).RestoreToDefault();
		}
	}
}