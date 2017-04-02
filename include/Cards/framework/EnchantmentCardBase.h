#pragma once

#include "FlowControl/enchantment/TieredEnchantments.h"
#include "Cards/CardAttributes.h"

namespace Cards
{
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
		static constexpr EnchantmentTiers normal_tier = EnchantmentTiers::kEnchantmentTier1;
		static constexpr EnchantmentTiers aura_tier = std::max({
			Enchant1::required_tier,
			Enchant2::required_tier,
			Enchant3::required_tier,
			Enchant4::required_tier,
			Enchant2::required_tier
		});

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