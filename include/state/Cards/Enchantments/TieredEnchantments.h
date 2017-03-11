#pragma once

#include "state/Cards/Enchantments/Enchantments.h"
#include "state/Cards/Enchantments/Tiers.h"

namespace state
{
	namespace Cards
	{
		class TieredEnchantments
		{
		public:
			using ContainerType = Enchantments::ContainerType;

			template <typename T>
			typename ContainerType::Identifier PushBack(T && item)
			{
				using EnchantmentType = std::decay_t<T>;
				return GetEnchantments<EnchantmentType::tier>().PushBack(item.apply_functor);
			}

			template <typename EnchantmentType, typename T>
			void Remove(T&& id)
			{
				return GetEnchantments<EnchantmentType::tier>().Remove(std::forward<T>(id));
			}

			template <typename EnchantmentType, typename T>
			bool Exists(T&& id) const
			{
				return GetEnchantments<EnchantmentType::tier>().Exists(std::forward<T>(id));
			}

			void ApplyAll(EnchantableStates & card)
			{
				tier1_.ApplyAll(card);
				tier2_.ApplyAll(card);
				tier3_.ApplyAll(card);
				aura_.ApplyAll(card);
			}

		private:
			template <int Tier> Enchantments & GetEnchantments();
			template <int Tier> const Enchantments & GetEnchantments() const;

		private:
			Enchantments tier1_;
			Enchantments tier2_;
			Enchantments tier3_;
			Enchantments aura_;
		};

		template <> inline Enchantments & TieredEnchantments::GetEnchantments<kEnchantmentTier1>() { return tier1_; }
		template <> inline const Enchantments & TieredEnchantments::GetEnchantments<kEnchantmentTier1>() const { return tier1_; }
		template <> inline Enchantments & TieredEnchantments::GetEnchantments<kEnchantmentTier2>() { return tier2_; }
		template <> inline const Enchantments & TieredEnchantments::GetEnchantments<kEnchantmentTier2>() const { return tier2_; }
		template <> inline Enchantments & TieredEnchantments::GetEnchantments<kEnchantmentTier3>() { return tier3_; }
		template <> inline const Enchantments & TieredEnchantments::GetEnchantments<kEnchantmentTier3>() const { return tier3_; }
		template <> inline Enchantments & TieredEnchantments::GetEnchantments<kEnchantmentAura>() { return aura_; }
		template <> inline const Enchantments & TieredEnchantments::GetEnchantments<kEnchantmentAura>() const { return aura_; }
	}
}