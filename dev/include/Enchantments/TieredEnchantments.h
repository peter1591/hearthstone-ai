#pragma once

#include "Enchantments/AttachedEnchantments.h"
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
	AttachedEnchantments & GetEnchantmentsByTier();

	template <int Tier>
	const AttachedEnchantments & GetEnchantmentsByTier() const;

	template <typename EnchantmentType>
	AttachedEnchantments & GetEnchantmentByType()
	{
		return GetEnchantmentsByTier<EnchantmentType::tier>();
	}

	template <typename EnchantmentType>
	AttachedEnchantments & GetEnchantmentByType() const
	{
		return GetEnchantmentsByTier<EnchantmentType::tier>();
	}

private:
	AttachedEnchantments tier1_;
	AttachedEnchantments tier2_;
	AttachedEnchantments tier3_;
	AttachedEnchantments aura_;
};

template <> AttachedEnchantments & TieredEnchantments::GetEnchantmentsByTier<kEnchantmentTier1>() { return tier1_; }
template <> AttachedEnchantments & TieredEnchantments::GetEnchantmentsByTier<kEnchantmentTier2>() { return tier2_; }
template <> AttachedEnchantments & TieredEnchantments::GetEnchantmentsByTier<kEnchantmentTier3>() { return tier3_; }
template <> AttachedEnchantments & TieredEnchantments::GetEnchantmentsByTier<kEnchantmentAura>() { return aura_; }

template <> const AttachedEnchantments & TieredEnchantments::GetEnchantmentsByTier<kEnchantmentTier1>() const { return tier1_; }
template <> const AttachedEnchantments & TieredEnchantments::GetEnchantmentsByTier<kEnchantmentTier2>() const { return tier2_; }
template <> const AttachedEnchantments & TieredEnchantments::GetEnchantmentsByTier<kEnchantmentTier3>() const { return tier3_; }
template <> const AttachedEnchantments & TieredEnchantments::GetEnchantmentsByTier<kEnchantmentAura>() const { return aura_; }
