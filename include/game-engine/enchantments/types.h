#pragma once

#include "game-engine/managed-list/list.h"

namespace GameEngine
{
	template <typename Target> class Enchantments;
	template <typename Target> class EnchantmentsOwner;
	template <typename Target> class EnchantmentsItemType;
	template <typename Target> class ManagedEnchantment;

	template <typename Target>
	class EnchantmentTypes
	{
	public:
		using Manager = Enchantments<Target>;
		using ManagerRawItem = EnchantmentsItemType<Target>;
		using ManagerManagedContainer = ManagedList<ManagerRawItem>;
		using ManagerManagedItem = typename ManagerManagedContainer::ManagedItem;

		using Owner = EnchantmentsOwner<Target>;
		using OwnerItem = ManagedEnchantment<Target>;
		using OwnerContainer = std::list<OwnerItem>;
		using OwnerToken = typename OwnerContainer::iterator;
	};
} // namespace GameEngine