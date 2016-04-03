#pragma once

#include <list>
#include "enchantment.h"

namespace GameEngine
{
	template <typename EnchantmentTarget>
	class EnchantmentOwner
	{
	public:
		bool IsEmpty() const;

		void RemoveOwnedEnchantments(EnchantmentTarget & owner);

		// hooks
		void EnchantmentAdded(Enchantment<EnchantmentTarget> * enchantment);
		void EnchantmentRemoved(Enchantment<EnchantmentTarget> * enchantment);

	private:
		std::list<Enchantment<EnchantmentTarget> *> enchantments;
	};
} // namespace GameEngine