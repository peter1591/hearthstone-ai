#pragma once

#include "state/State.h"

namespace Cards {
	struct NoAttribute {
		static void Apply(state::Cards::CardData & card_data) {}
	};
	struct Taunt {
		static void Apply(state::Cards::CardData & card_data) {
			card_data.enchantable_states.taunt = true;
		}
	};
	struct Shield {
		static void Apply(state::Cards::CardData & card_data) {
			card_data.enchantable_states.shielded = true;
		}
	};
	struct Charge {
		static void Apply(state::Cards::CardData & card_data) {
			card_data.enchantable_states.charge = true;
		}
	};
	template <int v> struct SpellDamage {
		static void Apply(state::Cards::CardData & card_data) {
			card_data.enchantable_states.spell_damage = v;
		}
	};
}