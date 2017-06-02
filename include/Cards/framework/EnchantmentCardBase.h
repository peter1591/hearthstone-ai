#pragma once

#include "FlowControl/enchantment/TieredEnchantments.h"
#include "Cards/CardAttributes.h"

namespace Cards
{
	namespace detail {
		template <typename T> class HasTierIfAura {
			typedef char One;
			typedef long Two;

			template <typename C> static One test(decltype(&C::tier_if_aura));
			template <typename C> static Two test(...);

		public:
			enum { value = sizeof(test<T>(0)) == sizeof(One) };
		};
	}

	class EnchantmentCardBase
	{
	public:
		EnchantmentCardBase() : apply_functor(nullptr), valid_this_turn(false), force_update_every_time(false) {}

		FlowControl::enchantment::Enchantments::ApplyFunctor apply_functor;
		bool valid_this_turn;
		bool force_update_every_time;
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
			// The first template parameter 'T' should be the client card class itself
			// And the rest of the template parameters are with the Enchant concept
			// Since, the Enchant concept has the 'tier_if_aura' field, we use it to detect misuse
			static_assert(detail::HasTierIfAura<T>::value == false);

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
		EnchantmentForThisTurn() : Enchantment<Ts...>() {
			this->valid_this_turn = true;
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

		void RemoveEnchantment() const {
			context.manipulate_.Card(card_ref).Enchant().Remove(
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
			// The first template parameter 'T' should be the client card class itself
			// And the rest of the template parameters are with the Enchant concept
			// Since, the Enchant concept has the 'tier_if_aura' field, we use it to detect misuse
			static_assert(detail::HasTierIfAura<T>::value == false);

			apply_functor = [](FlowControl::enchantment::Enchantments::ApplyFunctorContext const& context) {
				Enchant1::Apply(*context.stats_);
				Enchant2::Apply(*context.stats_);
				Enchant3::Apply(*context.stats_);
				Enchant4::Apply(*context.stats_);
				Enchant5::Apply(*context.stats_);
			};

			register_functor = [](FlowControl::Manipulate const& manipulate, state::CardRef card_ref,
					FlowControl::enchantment::Enchantments::IdentifierType id,
					FlowControl::enchantment::Enchantments::EventHookedEnchantment::AuxData & aux_data)
			{
				using EventType = typename T::EventType;
				manipulate.AddEvent<EventType>([card_ref, id, aux_data](typename EventType::Context const& context) mutable {
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