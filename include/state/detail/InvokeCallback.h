#pragma once

namespace FlowControl { class Manipulate; }

namespace state {
	namespace detail {
		template <CardType CardType, CardZone CardZone>
		struct InvokeCallback {
			static void Added(FlowControl::Manipulate & manipulate, state::CardRef card_ref, state::Cards::Card & card) {}
			static void Removed(FlowControl::Manipulate & manipulate, state::CardRef card_ref, state::Cards::Card & card) {}
		};

		template <CardType CardType>
		struct InvokeCallback<CardType, kCardZonePlay> {
			static void Added(FlowControl::Manipulate & manipulate, state::CardRef card_ref, state::Cards::Card & card) {
				card.SetJustPlayedFlag(true);
				card.GetRawData().added_to_play_zone(state::Cards::ZoneChangedContext{ manipulate, card_ref, card });
			}
			static void Removed(FlowControl::Manipulate & manipulate, state::CardRef card_ref, state::Cards::Card & card) {}
		};

		// TODO: added to deck zone callback
	}
}