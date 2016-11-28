#pragma once

#include <list>
#include <memory>

#include "game-engine/enchantments/types.h"

namespace GameEngine
{
	template <typename Target> class Enchantment;

	// Ensure the token are valid for all operations
	// Currently it's implemented by a std::list, and does NOT support any remove operation
	// This consumes more memory in game board, but does not have any memory footprint in MCTS tree
	template <typename EnchantmentTarget>
	class EnchantmentsHolder
	{
	private:
		using ContainerItemType = typename EnchantmentTypes<EnchantmentTarget>::HolderItem;
		using Container = typename EnchantmentTypes<EnchantmentTarget>::HolderContainer;
		using Token = typename EnchantmentTypes<EnchantmentTarget>::HolderToken;

	public:
		Token Add(ContainerItemType && item)
		{
			size_t token = container.size();
			container.push_back(std::move(item));
			return token;
		}

		ContainerItemType const& Get(Token token) const
		{
			return this->container[token];
		}

		void Remove(Token token)
		{
			// we do NOT remove any item, since we need to maintain the token's validity
			(void)token;
		}

	private:
		Container container;
	};

} // namespace GameEngine