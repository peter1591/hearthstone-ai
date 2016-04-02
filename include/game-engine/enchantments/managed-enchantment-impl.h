#pragma once

#include "managed-enchantment.h"

namespace GameEngine
{
	template<typename Owner>
	inline ManagedEnchantment<Owner>::ManagedEnchantment(Owner & owner, std::unique_ptr<EnchantmentType>&& enchantment)
		: owner(owner), enchantment(std::move(enchantment))
	{
	}

	template<typename Owner>
	inline bool ManagedEnchantment<Owner>::operator==(ManagedEnchantment<Owner> const& rhs) const
	{
		return (*(this->enchantment.get())) == (*(rhs.enchantment.get()));
	}

	template<typename Owner>
	inline bool ManagedEnchantment<Owner>::operator!=(ManagedEnchantment<Owner> const& rhs) const
	{
		return !(*this == rhs);
	}

	template<typename Owner>
	inline typename ManagedEnchantment<Owner>::EnchantmentType * ManagedEnchantment<Owner>::GetEnchantment() const
	{
		return this->enchantment.get();
	}

	template<typename Owner>
	inline Owner & ManagedEnchantment<Owner>::GetOwner() const
	{
		return this->owner;
	}

} // namespace GameEngine

template <typename Owner>
inline std::size_t std::hash<GameEngine::ManagedEnchantment<Owner>>::operator()(const argument_type &s) const
{
	return std::hash<GameEngine::ManagedEnchantment<Owner>::EnchantmentType>()(*s.GetEnchantment());
}