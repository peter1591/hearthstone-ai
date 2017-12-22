#pragma once

#include "state/State.h"

namespace Cards {

	using engine::FlowControl::enchantment::EnchantmentTiers;

	struct NoAttribute {
		static void Apply(state::Cards::CardData & card_data) {}
	};
	struct NullEnchant {
		static constexpr EnchantmentTiers tier_if_aura = EnchantmentTiers::kEnchantmentTier1;
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
		static constexpr EnchantmentTiers tier_if_aura = EnchantmentTiers::kEnchantmentTier1;
		static void Apply(state::Cards::EnchantableStates & stats) {
			stats.charge = true;
		}
	};

	struct Stealth {
		static void Apply(state::Cards::CardData & card_data) {
			card_data.enchanted_states.stealth = true;
		}
		static constexpr EnchantmentTiers tier_if_aura = EnchantmentTiers::kEnchantmentTier1;
		static void Apply(state::Cards::EnchantableStates & stats) {
			stats.stealth = true;
		}
	};

	struct Windfury {
		static void Apply(state::Cards::CardData & card_data) {
			card_data.enchanted_states.max_attacks_per_turn = 2;
		}
		static constexpr EnchantmentTiers tier_if_aura = EnchantmentTiers::kEnchantmentTier1;
		static void Apply(state::Cards::EnchantableStates & stats) {
			stats.max_attacks_per_turn = std::max(stats.max_attacks_per_turn, 2);
		}
	};

	struct ImmuneToSpellAndHeroPower {
		static void Apply(state::Cards::CardData & card_data) {
			card_data.enchanted_states.immune_to_spell = true;
		}
		static constexpr EnchantmentTiers tier_if_aura = EnchantmentTiers::kEnchantmentTier1;
		static void Apply(state::Cards::EnchantableStates & stats) {
			stats.immune_to_spell = true;
		}
	};

	template <int v> struct SpellDamage {
		static void Apply(state::Cards::CardData & card_data) {
			card_data.enchanted_states.spell_damage = v;
		}
		static constexpr EnchantmentTiers tier_if_aura = EnchantmentTiers::kEnchantmentTier1;
		static void Apply(state::Cards::EnchantableStates & stats) {
			stats.spell_damage += v;
		}
	};
	template <int v> struct Overload {
		static void Apply(state::Cards::CardData & card_data) {
			card_data.overload = v;
		}
	};

	template <int v> struct Attack {
		static constexpr EnchantmentTiers tier_if_aura = EnchantmentTiers::kEnchantmentTier2;
		static void Apply(state::Cards::EnchantableStates & stats) {
			stats.attack += v;
		}
	};
	template <int v> struct MaxHP {
		static constexpr EnchantmentTiers tier_if_aura = EnchantmentTiers::kEnchantmentTier2;
		static void Apply(state::Cards::EnchantableStates & stats) {
			stats.max_hp += v;
		}
	};
	template <int v> struct SetAttack {
		static constexpr EnchantmentTiers tier_if_aura = EnchantmentTiers::kEnchantmentTier2;
		static void Apply(state::Cards::EnchantableStates & stats) {
			stats.attack = v;
		}
	};
	template <int v> struct SetMaxHP {
		static constexpr EnchantmentTiers tier_if_aura = EnchantmentTiers::kEnchantmentTier2;
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
		static constexpr EnchantmentTiers tier_if_aura = EnchantmentTiers::kEnchantmentTier1;
		static void Apply(state::Cards::EnchantableStates & stats) {
			stats.poisonous = true;
		}
	};

	struct FreezeAttack {
		static void Apply(state::Cards::CardData & card_data) {
			card_data.enchanted_states.freeze_attack = true;
		}
		static constexpr EnchantmentTiers tier_if_aura = EnchantmentTiers::kEnchantmentTier1;
		static void Apply(state::Cards::EnchantableStates & stats) {
			stats.freeze_attack = true;
		}
	};

	struct CantAttackHero {
		static void Apply(state::Cards::CardData & card_data) {
			card_data.enchanted_states.cant_attack_hero = true;
		}
		static constexpr EnchantmentTiers tier_if_aura = EnchantmentTiers::kEnchantmentTier1;
		static void Apply(state::Cards::EnchantableStates & stats) {
			stats.cant_attack_hero = true;
		}
	};

	template <int v> struct Cost {
		static constexpr EnchantmentTiers tier_if_aura = EnchantmentTiers::kEnchantmentTier1;
		static void Apply(state::Cards::EnchantableStates & stats) {
			stats.cost += v;
		}
	};

	struct Immune {
		static constexpr EnchantmentTiers tier_if_aura = EnchantmentTiers::kEnchantmentTier1;
		static void Apply(state::Cards::EnchantableStates & stats) {
			stats.immune = true;
		}
	};

	template <state::PlayerSide player>
	struct SetPlayer {
		static constexpr EnchantmentTiers tier_if_aura = EnchantmentTiers::kEnchantmentTier1;
		static void Apply(state::Cards::EnchantableStates & stats) {
			stats.player.SetSide(player);
		}
	};
}