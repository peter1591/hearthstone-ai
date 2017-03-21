#pragma once

#include "state/State.h"

namespace Cards {
	struct NoAttribute {
		static void Apply(state::Cards::CardData & card_data) {}
	};
	struct Taunt {
		static void Apply(state::Cards::CardData & card_data) {
			card_data.taunt = true;
		}
	};
	struct Shield {
		static void Apply(state::Cards::CardData & card_data) {
			card_data.shielded = true;
		}
	};
	struct Charge {
		static void Apply(state::Cards::CardData & card_data) {
			card_data.charge = true;
		}
	};
	template <int v> struct SpellDamage {
		static void Apply(state::Cards::CardData & card_data) {
			card_data.enchanted_states.spell_damage = v;
		}
	};
	template <int v> struct Overload {
		static void Apply(state::Cards::CardData & card_data) {
			card_data.overload = v;
		}
	};
}