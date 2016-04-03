#pragma once

#include <memory>

namespace GameEngine
{
	template <typename Target> class Enchantment;
	template <typename EnchantmentTarget> class EnchantmentOwner;

	template <typename Target>
	class EnchantmentsItemType
	{
	public:
		EnchantmentsItemType(std::unique_ptr<Enchantment<Target>> && enchantment, EnchantmentOwner<Target> * owner) :
			enchantment(std::move(enchantment)), owner(owner)
		{
		}

		bool operator==(EnchantmentsItemType const& rhs) const { return *this->enchantment == *rhs.enchantment; }
		bool operator!=(EnchantmentsItemType const& rhs) const { return !(*this == rhs); }

		std::unique_ptr<Enchantment<Target>> enchantment;
		EnchantmentOwner<Target> * owner;
	};
} // namespace GameEngine