#pragma once

#include <memory>

#include "enchantments-holder.h"
#include "game-engine/enchantments/types.h"

namespace GameEngine
{
	template <typename Target> class Enchantment;
	template <typename EnchantmentTarget> class EnchantmentsOwner;

	template <typename Target>
	class EnchantmentsItemType
	{
	private:
		using Holder = typename EnchantmentTypes<Target>::Holder;
		using HolderToken = typename EnchantmentTypes<Target>::HolderToken;

	public:
		EnchantmentsItemType(HolderToken holder_token, EnchantmentsOwner<Target> * owner) :
			holder_token(holder_token), owner(owner), owner_token(nullptr)
		{
		}

		bool operator==(EnchantmentsItemType const& rhs) const = delete;
		bool operator!=(EnchantmentsItemType const& rhs) const = delete;

		bool EqualsTo(EnchantmentsItemType const& rhs, Holder const& lhs_holder, Holder const& rhs_holder) const
		{
			// TODO: we didn't compare enchantment owner here. should we?

			Enchantment<Target> * lhs_enchant = lhs_holder.Get(this->holder_token).get();
			Enchantment<Target> * rhs_enchant = rhs_holder.Get(rhs.holder_token).get();

			return *lhs_enchant == *rhs_enchant;
		}

		HolderToken holder_token;
		typename EnchantmentTypes<Target>::Owner * owner;
		typename EnchantmentTypes<Target>::OwnerToken * owner_token;
	};
} // namespace GameEngine