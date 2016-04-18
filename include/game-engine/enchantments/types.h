#pragma once

#include <list>
#include <vector>
#include <memory>
#include "game-engine/managed-list/list.h"

namespace GameEngine
{
	template <typename Target> class Enchantments;
	template <typename Target> class EnchantmentsOwner;
	template <typename Target> class EnchantmentsItemType;
	template <typename Target> class ManagedEnchantment;
	template <typename Target> class EnchantmentsHolder;

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

		using Holder = EnchantmentsHolder<Target>;
		using HolderItem = std::unique_ptr<Enchantment<Target>>;
		using HolderContainer = std::vector<HolderItem>;
		using HolderToken = size_t;
	};
} // namespace GameEngine