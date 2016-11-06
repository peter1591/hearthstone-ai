#pragma once

#include "Enchantments/Enchantments.h"
#include "Enchantments/Tiers.h"

class TieredEnchantments
{
public:
	typedef CloneableContainers::RemovablePtrVector<Enchantment::Base*> ContainerType;

	template <typename EnchantmentType>
	Enchantment::Base* Get(typename ContainerType::Identifier id)
	{
		return GetEnchantmentByType<EnchantmentType>().Get(id);
	}

	template <typename EnchantmentType, typename T>
	typename ContainerType::Identifier PushBack(T && item)
	{
		return GetEnchantmentByType<EnchantmentType>().PushBack(std::forward<T>(item));
	}

private:
	template <int Tier>
	Enchantments & GetEnchantmentsByTier();

	template <int Tier>
	const Enchantments & GetEnchantmentsByTier() const;

	template <typename EnchantmentType>
	Enchantments & GetEnchantmentByType()
	{
		return GetEnchantmentsByTier<EnchantmentType::tier>();
	}

	template <typename EnchantmentType>
	Enchantments & GetEnchantmentByType() const
	{
		return GetEnchantmentsByTier<EnchantmentType::tier>();
	}

private:
	Enchantments tier1_;
	Enchantments tier2_;
	Enchantments tier3_;
	Enchantments aura_;
};

template <> Enchantments & TieredEnchantments::GetEnchantmentsByTier<kEnchantmentTier1>() { return tier1_; }
template <> Enchantments & TieredEnchantments::GetEnchantmentsByTier<kEnchantmentTier2>() { return tier2_; }
template <> Enchantments & TieredEnchantments::GetEnchantmentsByTier<kEnchantmentTier3>() { return tier3_; }
template <> Enchantments & TieredEnchantments::GetEnchantmentsByTier<kEnchantmentAura>() { return aura_; }

template <> const Enchantments & TieredEnchantments::GetEnchantmentsByTier<kEnchantmentTier1>() const { return tier1_; }
template <> const Enchantments & TieredEnchantments::GetEnchantmentsByTier<kEnchantmentTier2>() const { return tier2_; }
template <> const Enchantments & TieredEnchantments::GetEnchantmentsByTier<kEnchantmentTier3>() const { return tier3_; }
template <> const Enchantments & TieredEnchantments::GetEnchantmentsByTier<kEnchantmentAura>() const { return aura_; }
