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
			Enchant5::required_tier
		});

		Enchantment() {
			// TODO: use SFINAE to make sure caller correctly pass itself as T
			//       T::required_tier must NOT exist

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
		EnchantmentForThisTurn() : Enchantment() {
			valid_this_turn = true;
		}
	};

	template <typename T>
	struct EventHookedEnchantmentHandler {
		using EventType = typename T::EventType;
		using ContextType = typename EventType::Context;

		EventHookedEnchantmentHandler(
			state::CardRef card_ref,
			ContextType const& context,
			FlowControl::enchantment::Enchantments::EventHookedEnchantment::AuxData & aux_data)
			: card_ref(card_ref), context(context), aux_data(aux_data)
		{
		}

		state::CardRef const card_ref;
		ContextType const& context;
		FlowControl::enchantment::Enchantments::EventHookedEnchantment::AuxData & aux_data;
	};

	template <
		typename T,
		typename Enchant1 = NullEnchant,
		typename Enchant2 = NullEnchant,
		typename Enchant3 = NullEnchant,
		typename Enchant4 = NullEnchant,
		typename Enchant5 = NullEnchant
	>
		struct EventHookedEnchantment
	{
		static constexpr EnchantmentTiers tier = EnchantmentTiers::kEnchantmentTier1;

		EventHookedEnchantment() {
			// TODO: use SFINAE to make sure caller correctly pass itself as T
			//       T::required_tier must NOT exist

			apply_functor = [](state::Cards::EnchantableStates & stats) {
				Enchant1::Apply(stats);
				Enchant2::Apply(stats);
				Enchant3::Apply(stats);
				Enchant4::Apply(stats);
				Enchant5::Apply(stats);
			};

			register_functor = [](FlowControl::Manipulate & manipulate, state::CardRef card_ref,
					FlowControl::enchantment::Enchantments::IdentifierType id,
					FlowControl::enchantment::Enchantments::EventHookedEnchantment::AuxData & aux_data)
			{
				using EventType = typename T::EventType;
				manipulate.AddEvent<EventType>([card_ref, id, aux_data](typename EventType::Context context) mutable {
					if (!context.manipulate_.GetCard(card_ref).GetEnchantmentHandler().Exists(
						FlowControl::enchantment::TieredEnchantments::IdentifierType{ T::tier, id })) return false;

					T::HandleEvent(EventHookedEnchantmentHandler<T>(card_ref, context, aux_data));
					return true;
				});
			};
		}

		FlowControl::enchantment::Enchantments::ApplyFunctor apply_functor;
		FlowControl::enchantment::Enchantments::EventHookedEnchantment::RegisterEventFunctor register_functor;
	};
}