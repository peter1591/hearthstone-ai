#pragma once

#include "Enchantments/AttachedEnchantments.h"
#include "Enchantments/DetachedEnchantments.h"
#include "Enchantments/Tiers.h"

class TieredEnchantments
{
public:
	typedef CloneableContainers::RemovablePtrVector<Enchantment::Base*> ContainerType;

	template <typename EnchantmentType>
	Enchantment::Base* Get(typename ContainerType::Identifier id)
	{
		return GetAttachedEnchantments<EnchantmentType::tier>().Get(id);
	}

	template <typename EnchantmentType, typename T,
		typename std::enable_if_t<IsAttachedEnchantmentType<EnchantmentType::tier>::value, nullptr_t> = nullptr >
	typename ContainerType::Identifier PushBack(T && item)
	{
		return GetAttachedEnchantments<EnchantmentType::tier>().PushBack(std::forward<T>(item));
	}

	template <typename EnchantmentType, typename T,
		typename std::enable_if_t<!IsAttachedEnchantmentType<EnchantmentType::tier>::value, nullptr_t> = nullptr >
	void PushBack(T && item)
	{
		return GetDetachedEnchantments<EnchantmentType::tier>().PushBack(std::forward<T>(item));
	}

private:
	template <int Tier> AttachedEnchantments & GetAttachedEnchantments();
	template <int Tier> const AttachedEnchantments & GetAttachedEnchantments() const;

	template <int Tier> DetachedEnchantments & GetDetachedEnchantments();
	template <int Tier> const DetachedEnchantments & GetDetachedEnchantments() const;

private:
	DetachedEnchantments tier1_;
	DetachedEnchantments tier2_;
	DetachedEnchantments tier3_;
	AttachedEnchantments aura_;
};

template <> DetachedEnchantments & TieredEnchantments::GetDetachedEnchantments<kEnchantmentTier1>() { return tier1_; }
template <> const DetachedEnchantments & TieredEnchantments::GetDetachedEnchantments<kEnchantmentTier1>() const { return tier1_; }
template <> DetachedEnchantments & TieredEnchantments::GetDetachedEnchantments<kEnchantmentTier2>() { return tier2_; }
template <> const DetachedEnchantments & TieredEnchantments::GetDetachedEnchantments<kEnchantmentTier2>() const { return tier2_; }
template <> DetachedEnchantments & TieredEnchantments::GetDetachedEnchantments<kEnchantmentTier3>() { return tier3_; }
template <> const DetachedEnchantments & TieredEnchantments::GetDetachedEnchantments<kEnchantmentTier3>() const { return tier3_; }
template <> AttachedEnchantments & TieredEnchantments::GetAttachedEnchantments<kEnchantmentAura>() { return aura_; }
template <> const AttachedEnchantments & TieredEnchantments::GetAttachedEnchantments<kEnchantmentAura>() const { return aura_; }
