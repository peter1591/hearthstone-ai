#pragma once

namespace engine {
	namespace FlowControl { class Manipulate; }
}

namespace state {
	namespace detail {
		template <CardType CardType, CardZone CardZone>
		struct InvokeCallback {
			static void Added(state::State & state, state::CardRef card_ref) {}
			static void Removed(state::State & state, state::CardRef card_ref) {}
		};

		template <CardType CardType>
		struct InvokeCallback<CardType, kCardZonePlay> {
			static void Added(state::State & state, state::CardRef card_ref);
			static void Removed(state::State & state, state::CardRef card_ref) {}
		};

		template <CardType CardType>
		struct InvokeCallback<CardType, kCardZoneHand> {
			static void Added(state::State & state, state::CardRef card_ref);
			static void Removed(state::State & state, state::CardRef card_ref) {}
		};
	}
}