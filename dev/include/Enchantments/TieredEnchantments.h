#pragma once

#include "Enchantments/Enchantments.h"
#include "Enchantments/Tiers.h"

class TieredEnchantments
{
public:
	typedef CloneableContainers::RemovablePtrVector<Enchantment::Base*> ContainerType;

	template <typename EnchantmentType, typename T>
	typename ContainerType::Identifier PushBack(T && item)
	{
		return GetEnchantments<EnchantmentType::tier>().PushBack(std::forward<T>(item));
	}

	template <typename EnchantmentType, typename T>
	Enchantment::Base* Get(T&& id)
	{
		return GetEnchantments<EnchantmentType::tier>().Get(std::forward<T>(id));
	}

	template <typename EnchantmentType, typename T>
	void Remove(T&& id)
	{
		return GetEnchantments<EnchantmentType::tier>().Remove(std::forward<T>(id));
	}

private:
	template <int Tier> AttachedEnchantments & GetEnchantments();
	template <int Tier> const AttachedEnchantments & GetEnchantments() const;

private:
	AttachedEnchantments tier1_;
	AttachedEnchantments tier2_;
	AttachedEnchantments tier3_;
	AttachedEnchantments aura_;
};

template <> AttachedEnchantments & TieredEnchantments::GetEnchantments<kEnchantmentTier1>() { return tier1_; }
template <> const AttachedEnchantments & TieredEnchantments::GetEnchantments<kEnchantmentTier1>() const { return tier1_; }
template <> AttachedEnchantments & TieredEnchantments::GetEnchantments<kEnchantmentTier2>() { return tier2_; }
template <> const AttachedEnchantments & TieredEnchantments::GetEnchantments<kEnchantmentTier2>() const { return tier2_; }
template <> AttachedEnchantments & TieredEnchantments::GetEnchantments<kEnchantmentTier3>() { return tier3_; }
template <> const AttachedEnchantments & TieredEnchantments::GetEnchantments<kEnchantmentTier3>() const { return tier3_; }
template <> AttachedEnchantments & TieredEnchantments::GetEnchantments<kEnchantmentAura>() { return aura_; }
template <> const AttachedEnchantments & TieredEnchantments::GetEnchantments<kEnchantmentAura>() const { return aura_; }
