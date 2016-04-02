#pragma once

#include "enchantment.h"
#include "owner.h"

namespace GameEngine
{
	template <typename Owner>
	class ManagedEnchantment
	{
		friend std::hash<ManagedEnchantment<Owner>>;

	private:
		typedef Enchantment<Owner> EnchantmentType;

	public:
		ManagedEnchantment(Owner & owner, std::unique_ptr<EnchantmentType> && enchantment);

		bool operator==(ManagedEnchantment<Owner> const& rhs) const;
		bool operator!=(ManagedEnchantment<Owner> const& rhs) const;

		EnchantmentType * GetEnchantment() const;
		Owner & GetOwner() const;

	private:
		Owner & owner;
		std::unique_ptr<EnchantmentType> enchantment;
	};
} // namespace GameEngine

namespace std {
	template <typename Owner> struct hash<GameEngine::ManagedEnchantment<Owner> > {
		typedef GameEngine::ManagedEnchantment<Owner> argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const;
	};
}