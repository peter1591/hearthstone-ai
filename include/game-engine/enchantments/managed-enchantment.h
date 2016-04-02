#pragma once

#include "enchantment.h"
#include "owner.h"

namespace GameEngine
{
	template <typename Owner>
	class ManagedEnchantment
	{
	private:
		typedef Enchantment<Owner> EnchantmentType;

	public:
		ManagedEnchantment(Owner & owner, std::unique_ptr<EnchantmentType> && enchantment);

		EnchantmentType * GetEnchantment() const { return this->enchantment.get(); }
		Owner & GetOwner() const { return this->owner; }

	private:
		Owner & owner;
		std::unique_ptr<EnchantmentType> enchantment;
	};
} // namespace GameEngine