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
		using Owner = typename EnchantmentTypes<Target>::Owner;
		using OwnerToken = typename EnchantmentTypes<Target>::OwnerToken;

	public:
		EnchantmentsItemType(HolderToken holder_token) :
			holder_token(holder_token), owner(nullptr), owner_token(nullptr)
		{
		}

		void SetOwner(typename EnchantmentTypes<Target>::Owner * owner, OwnerToken owner_token)
		{
			this->owner = owner;
			this->owner_token = std::make_unique<OwnerToken>(owner_token);
		}

		void UnsetOwner()
		{
			this->owner = nullptr;
			this->owner_token.reset(nullptr);
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

		HolderToken const& GetHolderToken() const { return this->holder_token; }
		Owner * GetOwner() const { return this->owner; }
		OwnerToken GetOwnerToken() const { return *this->owner_token; }

	private:
		HolderToken holder_token;
		Owner * owner;
		std::unique_ptr<OwnerToken> owner_token;
	};
} // namespace GameEngine