#pragma once

#include "state/State.h"

namespace Cards {

	using FlowControl::enchantment::EnchantmentTiers;

	struct NoAttribute {
		static void Apply(state::Cards::CardData & card_data) {}
	};
	struct NullEnchant {
		static constexpr EnchantmentTiers required_tier = EnchantmentTiers::kEnchantmentTier1;
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
		static constexpr EnchantmentTiers required_tier = EnchantmentTiers::kEnchantmentTier1;
		static void Apply(state::Cards::EnchantableStates & stats) {
			stats.charge = true;
		}
	};

	struct Stealth {
		static void Apply(state::Cards::CardData & card_data) {
			card_data.enchanted_states.stealth = true;
		}
		static constexpr EnchantmentTiers required_tier = EnchantmentTiers::kEnchantmentTier1;
		static void Apply(state::Cards::EnchantableStates & stats) {
			stats.stealth = true;
		}
	};

	struct Windfury {
		static void Apply(state::Cards::CardData & card_data) {
			card_data.enchanted_states.max_attacks_per_turn = 2;
		}
		static constexpr EnchantmentTiers required_tier = EnchantmentTiers::kEnchantmentTier1;
		static void Apply(state::Cards::EnchantableStates & stats) {
			stats.max_attacks_per_turn = std::max(stats.max_attacks_per_turn, 2);
		}
	};

	struct ImmuneToSpellAndHeroPower {
		static void Apply(state::Cards::CardData & card_data) {
			card_data.enchanted_states.immune_to_spell = true;
		}
		static constexpr EnchantmentTiers required_tier = EnchantmentTiers::kEnchantmentTier1;
		static void Apply(state::Cards::EnchantableStates & stats) {
			stats.immune_to_spell = true;
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
		static constexpr EnchantmentTiers required_tier = EnchantmentTiers::kEnchantmentTier2;
		static void Apply(state::Cards::EnchantableStates & stats) {
			stats.attack += v;
		}
	};
	template <int v> struct MaxHP {
		static constexpr EnchantmentTiers required_tier = EnchantmentTiers::kEnchantmentTier2;
		static void Apply(state::Cards::EnchantableStates & stats) {
			stats.max_hp += v;
		}
	};
	template <int v> struct SetAttack {
		static constexpr EnchantmentTiers required_tier = EnchantmentTiers::kEnchantmentTier2;
		static void Apply(state::Cards::EnchantableStates & stats) {
			stats.attack = v;
		}
	};
	template <int v> struct SetMaxHP {
		static constexpr EnchantmentTiers required_tier = EnchantmentTiers::kEnchantmentTier2;
		static void Apply(state::Cards::EnchantableStates & stats) {
			stats.max_hp = v;
		}
	};

	struct CantAttack {
		static void Apply(state::Cards::CardData & card_data) {
			card_data.cant_attack = true;
		}
	};

	struct Poisonous {
		static void Apply(state::Cards::CardData & card_data) {
			card_data.enchanted_states.poisonous = true;
		}
		static constexpr EnchantmentTiers required_tier = EnchantmentTiers::kEnchantmentTier1;
		static void Apply(state::Cards::EnchantableStates & stats) {
			stats.poisonous = true;
		}
	};
}