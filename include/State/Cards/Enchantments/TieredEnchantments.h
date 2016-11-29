#pragma once

#include "State/Cards/Enchantments/Enchantments.h"
#include "State/Cards/Enchantments/Tiers.h"

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
				return GetEnchantments<EnchantmentType::tier>().PushBack(std::forward<T>(item));
			}

			template <typename T>
			typename ContainerType::Identifier PushBackAuraEnchant(T && item)
			{
				return GetEnchantments<kEnchantmentAura>().PushBackFunctor(std::forward<T>(item));
			}

			template <typename EnchantmentType, typename T>
			void Remove(T&& id)
			{
				return GetEnchantments<EnchantmentType::tier>().Remove(std::forward<T>(id));
			}

			template <typename T>
			void RemoveAuraEnchant(T&& id)
			{
				return GetEnchantments<kEnchantmentAura>().Remove(std::forward<T>(id));
			}

			void ApplyAll(Card & card)
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

		template <> Enchantments & TieredEnchantments::GetEnchantments<kEnchantmentTier1>() { return tier1_; }
		template <> const Enchantments & TieredEnchantments::GetEnchantments<kEnchantmentTier1>() const { return tier1_; }
		template <> Enchantments & TieredEnchantments::GetEnchantments<kEnchantmentTier2>() { return tier2_; }
		template <> const Enchantments & TieredEnchantments::GetEnchantments<kEnchantmentTier2>() const { return tier2_; }
		template <> Enchantments & TieredEnchantments::GetEnchantments<kEnchantmentTier3>() { return tier3_; }
		template <> const Enchantments & TieredEnchantments::GetEnchantments<kEnchantmentTier3>() const { return tier3_; }
		template <> Enchantments & TieredEnchantments::GetEnchantments<kEnchantmentAura>() { return aura_; }
		template <> const Enchantments & TieredEnchantments::GetEnchantments<kEnchantmentAura>() const { return aura_; }
	}
}