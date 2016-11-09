#pragma once

enum EnchantmentTiers
{
	kEnchantmentTier1, // detached enchantments
	kEnchantmentTier2, // detached enchantments
	kEnchantmentTier3, // detached enchantments
	kEnchantmentAura   // attached enchantments
};

template <int tier> struct IsAttachedEnchantmentType;

template <> struct IsAttachedEnchantmentType<kEnchantmentTier1> { static constexpr bool value = false; };
template <> struct IsAttachedEnchantmentType<kEnchantmentTier2> { static constexpr bool value = false; };
template <> struct IsAttachedEnchantmentType<kEnchantmentTier3> { static constexpr bool value = false; };
template <> struct IsAttachedEnchantmentType<kEnchantmentAura> { static constexpr bool value = true; };