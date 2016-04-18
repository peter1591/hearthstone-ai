#pragma once

#include <memory>

#include "game-engine/enchantments/types.h"

namespace GameEngine
{
	template <typename Target> class Enchantment;
	template <typename EnchantmentTarget> class EnchantmentsOwner;

	template <typename Target>
	class EnchantmentsItemType
	{
	public:
		EnchantmentsItemType(std::unique_ptr<Enchantment<Target>> && enchantment, EnchantmentsOwner<Target> * owner) :
			enchantment(std::move(enchantment)), owner(owner), owner_token(nullptr)
		{
		}

		bool operator==(EnchantmentsItemType const& rhs) const = delete;
		bool operator!=(EnchantmentsItemType const& rhs) const = delete;

		bool EqualsTo(EnchantmentsItemType const& rhs) const
		{
			return *this->enchantment == *rhs.enchantment;
		}

		std::unique_ptr<Enchantment<Target>> enchantment;
		typename EnchantmentTypes<Target>::Owner * owner;
		typename EnchantmentTypes<Target>::OwnerToken * owner_token;
	};
} // namespace GameEngine

namespace std {
	template <typename Target>
	struct hash<GameEngine::EnchantmentsItemType<Target>> {
		typedef GameEngine::EnchantmentsItemType<Target> argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			return std::hash<GameEngine::Enchantment<Target>>()(*s.enchantment);
		}
	};
}