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

			// TODO: if ZoneTo = hand, and it's full, go to graveyard
			state_.GetZoneChanger<state::kCardTypeMinion, Zone>(
				FlowControl::Manipulate(state_, flow_context_), card_ref_
				).ChangeTo<ZoneTo>(to_player);

			state_.GetMutableCard(card_ref_).RestoreToDefault();
		}
	}
}