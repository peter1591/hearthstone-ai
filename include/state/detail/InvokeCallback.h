#pragma once

namespace state {
	namespace detail {
		template <CardType CardType, CardZone CardZone>
		struct InvokeCallback {
			static void Added(state::State & state, state::CardRef card_ref, state::Cards::Card & card) {}
			static void Removed(state::State & state, state::CardRef card_ref, state::Cards::Card & card) {}
		};

		template <CardType CardType>
		struct InvokeCallback<CardType, kCardZonePlay> {
			static void Added(state::State & state, state::CardRef card_ref, state::Cards::Card & card) {
				card.SetJustPlayedFlag(true);
				card.GetRawData().added_to_play_zone(state::Cards::ZoneChangedContext{ state, card_ref, card });
			}
			static void Removed(state::State & state, state::CardRef card_ref, state::Cards::Card & card) {}
		};

		// TODO: added to deck zone callback
	}
}