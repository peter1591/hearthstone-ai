#pragma once

#include "FlowControl/enchantment/TieredEnchantments.h"
#include "Cards/CardAttributes.h"

namespace Cards
{
	using FlowControl::enchantment::EnchantmentTiers;

	class EnchantmentCardBase
	{
	public:
		EnchantmentCardBase() : apply_functor(nullptr), valid_this_turn(false) {}

		FlowControl::enchantment::Enchantments::ApplyFunctor apply_functor;
		bool valid_this_turn;
	};

	template <
		typename T,
		typename Enchant1 = NullEnchant,
		typename Enchant2 = NullEnchant,
		typename Enchant3 = NullEnchant,
		typename Enchant4 = NullEnchant,
		typename Enchant5 = NullEnchant
	>
		struct Enchantment : public EnchantmentCardBase
	{
		Enchantment() {
			apply_functor = [](state::Cards::EnchantableStates & stats) {
				Enchant1::Apply(stats);
				Enchant2::Apply(stats);
				Enchant3::Apply(stats);
				Enchant4::Apply(stats);
				Enchant5::Apply(stats);
			};
		}
	};

	template <typename... Ts>
	struct EnchantmentForThisTurn : public Enchantment<Ts...>
	{
		EnchantmentForThisTurn() {
			valid_this_turn = true;
		}
	};
}