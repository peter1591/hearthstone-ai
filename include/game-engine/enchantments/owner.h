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
		using Manager = typename ManagedEnchantment<EnchantmentTarget>::Manager;
		using ManagedItem =typename Manager::ManagedItem;

		EnchantmentOwner() {}
		EnchantmentOwner(EnchantmentOwner<EnchantmentTarget> const& rhs) = delete;
		EnchantmentOwner(EnchantmentOwner<EnchantmentTarget> && rhs) = delete;
		EnchantmentOwner<EnchantmentTarget> & operator=(EnchantmentOwner<EnchantmentTarget> const& rhs) = delete;
		EnchantmentOwner<EnchantmentTarget> & operator=(EnchantmentOwner<EnchantmentTarget> && rhs) = delete;

		bool IsEmpty() const;

		void RemoveOwnedEnchantments();

		// hooks
		void EnchantmentAdded(ManagedEnchantment<EnchantmentTarget> const& managed_enchantment);
		void EnchantmentRemoved(ManagedItem managed_enchantment);

	private:
		std::list<ManagedEnchantment<EnchantmentTarget>> enchantments;
	};
} // namespace GameEngine