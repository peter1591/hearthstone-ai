#pragma once

#include "state/State.h"

namespace Cards {

	struct NoAttribute {
		static void Apply(state::Cards::CardData & card_data) {}
	};
	struct NullEnchant {
		static void Apply(state::Cards::EnchantableStates & stats) {}
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
			card_data.enchanted_states.charge = true;
		}
		static void Apply(state::Cards::EnchantableStates & stats) {
			stats.charge = true;
		}
	};

	struct Stealth {
		static void Apply(state::Cards::CardData & card_data) {
			card_data.enchanted_states.stealth = true;
		}
		static void Apply(state::Cards::EnchantableStates & stats) {
			stats.stealth = true;
		}
	};

	struct Windfury {
		static void Apply(state::Cards::CardData & card_data) {
			card_data.enchanted_states.windfury = true;
		}
		static void Apply(state::Cards::EnchantableStates & stats) {
			stats.windfury = true;
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

	template <int v> struct Attack {
		static void Apply(state::Cards::EnchantableStates & stats) {
			stats.attack += v;
		}
	};
	template <int v> struct MaxHP {
		static void Apply(state::Cards::EnchantableStates & stats) {
			stats.max_hp += v;
		}
	};
	template <int v> struct SetAttack {
		static void Apply(state::Cards::EnchantableStates & stats) {
			stats.attack = v;
		}
	};
	template <int v> struct SetMaxHP {
		static void Apply(state::Cards::EnchantableStates & stats) {
			stats.max_hp = v;
		}
	};

	struct CantAttack {
		static void Apply(state::Cards::CardData & card_data) {
			card_data.cant_attack = true;
		}
	};
}