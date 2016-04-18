#pragma once

#include <list>
#include "enchantment.h"
#include "managed-enchantment.h"

namespace GameEngine
{
	template <typename EnchantmentTarget>
	class EnchantmentsOwner
	{
	public:
		using Manager = typename ManagedEnchantment<EnchantmentTarget>::Manager;
		using ManagedItem = typename Manager::ManagedItem;
		using Container = typename Enchantments<EnchantmentTarget>::OwnerContainer;
		using Token = typename Enchantments<EnchantmentTarget>::OwnerToken;

		EnchantmentsOwner() {}
		EnchantmentsOwner(EnchantmentsOwner<EnchantmentTarget> const& rhs) = delete;
		EnchantmentsOwner(EnchantmentsOwner<EnchantmentTarget> && rhs) = delete;
		EnchantmentsOwner<EnchantmentTarget> & operator=(EnchantmentsOwner<EnchantmentTarget> const& rhs) = delete;
		EnchantmentsOwner<EnchantmentTarget> & operator=(EnchantmentsOwner<EnchantmentTarget> && rhs) = delete;

		bool IsEmpty() const;

		void RemoveOwnedEnchantments();

		// hooks
		Token EnchantmentAdded(ManagedEnchantment<EnchantmentTarget> const& managed_enchantment);
		void EnchantmentRemoved(ManagedItem managed_enchantment);

	private:
		std::list<ManagedEnchantment<EnchantmentTarget>> enchantments;
	};
} // namespace GameEngine