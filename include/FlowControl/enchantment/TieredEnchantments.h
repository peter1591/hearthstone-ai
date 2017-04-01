#pragma once

#include "FlowControl/enchantment/Enchantments.h"

namespace FlowControl
{
	namespace enchantment
	{
		enum EnchantmentTiers
		{
			kEnchantmentTier1,
			kEnchantmentTier2,
			kEnchantmentTier3
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
			typename IdentifierType PushBackAuraEnchantment()
			{
				constexpr EnchantmentTiers tier = EnchantmentType::tier;
				Enchantments::IdentifierType id = GetEnchantments<tier>().PushBackAuraEnchantment<EnchantmentType>();
				return IdentifierType{ tier, id };
			}

			template <typename EnchantmentType>
			void PushBackNormalEnchantment(state::State const& state)
			{
				GetEnchantments<EnchantmentType::tier>().PushBackNormalEnchantment<EnchantmentType>(state);
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

			void AfterCopied()
			{
				tier1_.AfterCopied();
				tier2_.AfterCopied();
				tier3_.AfterCopied();
			}

			void ApplyAll(state::State const& state, state::Cards::EnchantableStates & stats)
			{
				tier1_.ApplyAll(state, stats);
				tier2_.ApplyAll(state, stats);
				tier3_.ApplyAll(state, stats);
			}

			bool NeedUpdate() const {
				if (tier1_.NeedUpdate()) return true;
				if (tier2_.NeedUpdate()) return true;
				if (tier3_.NeedUpdate()) return true;
				return false;
			}

			void FinishedUpdate() {
				tier1_.FinishedUpdate();
				tier2_.FinishedUpdate();
				tier3_.FinishedUpdate();
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