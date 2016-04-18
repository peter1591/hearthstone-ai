#pragma once

#include <memory>

namespace GameEngine
{
	template <typename Target> class Enchantment;
	template <typename EnchantmentTarget> class EnchantmentsOwner;

	template <typename Target>
	class EnchantmentsItemType
	{
	public:
		EnchantmentsItemType(std::unique_ptr<Enchantment<Target>> && enchantment, EnchantmentsOwner<Target> * owner) :
			enchantment(std::move(enchantment)), owner(owner)
		{
		}

		bool operator==(EnchantmentsItemType const& rhs) const
		{
			return *this->enchantment == *rhs.enchantment;
		}
		bool operator!=(EnchantmentsItemType const& rhs) const { return !(*this == rhs); }

		std::unique_ptr<Enchantment<Target>> enchantment;
		EnchantmentsOwner<Target> * owner;

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