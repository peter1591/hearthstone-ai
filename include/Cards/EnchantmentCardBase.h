#pragma once

#include "FlowControl/enchantment/TieredEnchantments.h"

namespace Cards
{
	using FlowControl::enchantment::EnchantmentTiers;

	class EnchantmentCardBase
	{
	public:
		FlowControl::enchantment::Enchantments::ApplyFunctor apply_functor;
	};

	struct NullEnchant {
		static void Apply(state::Cards::EnchantableStates & stats) {
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

	template <
		typename Enchant1 = NullEnchant,
		typename Enchant2 = NullEnchant,
		typename Enchant3 = NullEnchant,
		typename Enchant4 = NullEnchant,
		typename Enchant5 = NullEnchant
	>
	struct Enchantment: public EnchantmentCardBase
	{
		Enchantment() {
			apply_functor = [](auto& stats) {
				Enchant1::Apply(stats);
				Enchant2::Apply(stats);
				Enchant3::Apply(stats);
				Enchant4::Apply(stats);
				Enchant5::Apply(stats);
			};
		}
	};
}