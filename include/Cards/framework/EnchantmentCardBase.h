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
			Enchant1::tier_if_aura,
			Enchant2::tier_if_aura,
			Enchant3::tier_if_aura,
			Enchant4::tier_if_aura,
			Enchant5::tier_if_aura
		});

		Enchantment() {
			// TODO: use SFINAE to make sure caller correctly pass itself as T
			//       T::tier_if_aura must NOT exist

			apply_functor = [](FlowControl::enchantment::Enchantments::ApplyFunctorContext const& context) {
				Enchant1::Apply(*context.stats_);
				Enchant2::Apply(*context.stats_);
				Enchant3::Apply(*context.stats_);
				Enchant4::Apply(*context.stats_);
				Enchant5::Apply(*context.stats_);
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
			FlowControl::enchantment::Enchantments::IdentifierType enchant_id,
			ContextType const& context,
			FlowControl::enchantment::Enchantments::EventHookedEnchantment::AuxData & aux_data)
			: card_ref(card_ref), enchant_id(enchant_id), context(context), aux_data(aux_data)
		{
		}

		void RemoveEnchantment() {
			FlowControl::Manipulate & manipulate = context.manipulate_;
			manipulate.Card(card_ref).Enchant().Remove(
				FlowControl::enchantment::TieredEnchantments::IdentifierType{ T::tier, enchant_id });
		}

		state::CardRef const card_ref;
		FlowControl::enchantment::Enchantments::IdentifierType enchant_id;
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
			//       T::tier_if_aura must NOT exist

			apply_functor = [](FlowControl::enchantment::Enchantments::ApplyFunctorContext const& context) {
				Enchant1::Apply(*context.stats_);
				Enchant2::Apply(*context.stats_);
				Enchant3::Apply(*context.stats_);
				Enchant4::Apply(*context.stats_);
				Enchant5::Apply(*context.stats_);
			};

			register_functor = [](FlowControl::Manipulate & manipulate, state::CardRef card_ref,
					FlowControl::enchantment::Enchantments::IdentifierType id,
					FlowControl::enchantment::Enchantments::EventHookedEnchantment::AuxData & aux_data)
			{
				using EventType = typename T::EventType;
				manipulate.AddEvent<EventType>([card_ref, id, aux_data](typename EventType::Context context) mutable {
					if (!context.manipulate_.Card(card_ref).Enchant().Exists(
						FlowControl::enchantment::TieredEnchantments::IdentifierType{ T::tier, id })) return false;

					T::HandleEvent(EventHookedEnchantmentHandler<T>(card_ref, id, context, aux_data));
					return true;
				});
			};
		}

		FlowControl::enchantment::Enchantments::ApplyFunctor apply_functor;
		FlowControl::enchantment::Enchantments::EventHookedEnchantment::RegisterEventFunctor register_functor;
	};
}