#pragma once

#include <list>
#include "enchantment.h"
#include "managed-enchantment.h"

namespace GameEngine
{
	template <typename EnchantmentTarget>
	class EnchantmentOwner
	{
	public:
		bool IsEmpty() const;

		void RemoveOwnedEnchantments();

		// hooks
		void EnchantmentAdded(ManagedEnchantment<EnchantmentTarget> const& managed_enchantment);
		void EnchantmentRemoved(EnchantmentsItemType<EnchantmentTarget> const& managed_enchantment);

	private:
		std::list<ManagedEnchantment<EnchantmentTarget>> enchantments;
	};
} // namespace GameEngine