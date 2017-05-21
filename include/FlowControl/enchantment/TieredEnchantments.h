#pragma once

#include "FlowControl/enchantment/Enchantments.h"

namespace FlowControl
{
	namespace enchantment
	{
		enum EnchantmentTiers
		{
			kEnchantmentTier1,
			kEnchantmentTier2, // attack/health auras
			kEnchantmentTier3 // enchantment ALWAYS applied last (e.g., Lightspawn)
		};

		class TieredEnchantments
		{
		public:
			using ContainerType = Enchantments::ContainerType;

			struct IdentifierType {
				EnchantmentTiers tier;
				Enchantments::IdentifierType id;
			};

			template <typename EnchantmentType>
			typename IdentifierType PushBackAuraEnchantment(EnchantmentType&& enchantment)
			{
				constexpr EnchantmentTiers tier = EnchantmentType::aura_tier;
				Enchantments::IdentifierType id = GetEnchantments<tier>().PushBackAuraEnchantment(std::forward<EnchantmentType>(enchantment));
				return IdentifierType{ tier, id };
			}

			template <typename EnchantmentType>
			void PushBackNormalEnchantment(state::State const& state, EnchantmentType&& enchantment)
			{
				GetEnchantments<EnchantmentType::normal_tier>().PushBackNormalEnchantment<EnchantmentType>(state, std::forward<EnchantmentType>(enchantment));
			}

			template <typename EnchantmentType>
			typename IdentifierType PushBackEventHookedEnchantment(FlowControl::Manipulate & manipulate, state::CardRef card_ref,
				EnchantmentType&& enchant, enchantment::Enchantments::EventHookedEnchantment::AuxData const& aux_data)
			{
				constexpr EnchantmentTiers tier = EnchantmentType::tier;
				Enchantments::IdentifierType id = GetEnchantments<tier>()
					.PushBackEventHookedEnchantment(manipulate, card_ref, std::forward<EnchantmentType>(enchant), aux_data);
				return IdentifierType{ tier, id };
			}

			void Remove(IdentifierType id)
			{
				switch (id.tier) {
				case kEnchantmentTier1:
					return tier1_.Remove(id.id);
				case kEnchantmentTier2:
					return tier2_.Remove(id.id);
				case kEnchantmentTier3:
					return tier3_.Remove(id.id);
				}
				assert(false);
			}

			bool Exists(IdentifierType id) const
			{
				switch (id.tier) {
				case kEnchantmentTier1:
					return tier1_.Exists(id.id);
				case kEnchantmentTier2:
					return tier2_.Exists(id.id);
				case kEnchantmentTier3:
					return tier3_.Exists(id.id);
				}
				assert(false);
				return false;
			}

			void Clear()
			{
				tier1_.Clear();
				tier2_.Clear();
				tier3_.Clear();
			}

			void AfterCopied(FlowControl::Manipulate & manipulate, state::CardRef card_ref)
			{
				tier1_.AfterCopied(manipulate, card_ref);
				tier2_.AfterCopied(manipulate, card_ref);
				tier3_.AfterCopied(manipulate, card_ref);
			}

			void ApplyAll(state::State const& state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::EnchantableStates & stats)
			{
				tier1_.ApplyAll(state, flow_context, card_ref, stats);
				tier2_.ApplyAll(state, flow_context, card_ref, stats);
				tier3_.ApplyAll(state, flow_context, card_ref, stats);
			}

			bool NeedUpdate(state::State & state) const {
				if (tier1_.NeedUpdate(state)) return true;
				if (tier2_.NeedUpdate(state)) return true;
				if (tier3_.NeedUpdate(state)) return true;
				return false;
			}

			void FinishedUpdate(state::State & state) {
				tier1_.FinishedUpdate(state);
				tier2_.FinishedUpdate(state);
				tier3_.FinishedUpdate(state);
			}

		private:
			template <int Tier> Enchantments & GetEnchantments();
			template <int Tier> const Enchantments & GetEnchantments() const;

		private:
			Enchantments tier1_;
			Enchantments tier2_;
			Enchantments tier3_;
		};

		template <> inline Enchantments & TieredEnchantments::GetEnchantments<kEnchantmentTier1>() { return tier1_; }
		template <> inline const Enchantments & TieredEnchantments::GetEnchantments<kEnchantmentTier1>() const { return tier1_; }
		template <> inline Enchantments & TieredEnchantments::GetEnchantments<kEnchantmentTier2>() { return tier2_; }
		template <> inline const Enchantments & TieredEnchantments::GetEnchantments<kEnchantmentTier2>() const { return tier2_; }
		template <> inline Enchantments & TieredEnchantments::GetEnchantments<kEnchantmentTier3>() { return tier3_; }
		template <> inline const Enchantments & TieredEnchantments::GetEnchantments<kEnchantmentTier3>() const { return tier3_; }
	}
}