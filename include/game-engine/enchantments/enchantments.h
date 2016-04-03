#pragma once

#include <memory>

#include "game-engine/managed-list/list.h"

namespace GameEngine {

class MinionData;
template <typename Target> class Enchantment;
class EnchantmentOwner;
class Minion;

template <typename Target>
class EnchantmentsItemType
{
public:
	EnchantmentsItemType(std::unique_ptr<Enchantment<Target>> && enchantment, EnchantmentOwner * owner) :
		enchantment(std::move(enchantment)), owner(owner)
	{
	}

	bool operator==(EnchantmentsItemType const& rhs) const { return *this->enchantment == *rhs.enchantment; }
	bool operator!=(EnchantmentsItemType const& rhs) const { return !(*this == rhs); }

	std::unique_ptr<Enchantment<Target>> enchantment;
	EnchantmentOwner * owner;
};

// Maintains the lifetime of Enchantment
template <typename Target>
class Enchantments
{
	friend std::hash<Enchantments>;

private:
	typedef Enchantment<Target> EnchantmentType;
	typedef EnchantmentsItemType<Target> ItemType;
	typedef typename ManagedList<ItemType> container_type;

public:
	Enchantments(Target & target) : target(target) {}

	Enchantments(Enchantments<Target> const& rhs) = delete;
	Enchantments<Target> operator=(Enchantments<Target> const& rhs) = delete;
	Enchantments(Target & target, Enchantments<Target> && rhs);
	Enchantments<Target> & operator=(Enchantments<Target> && rhs) = delete;

	bool operator==(Enchantments const& rhs) const;
	bool operator!=(Enchantments const& rhs) const;

	void Add(std::unique_ptr<EnchantmentType> && enchantment, EnchantmentOwner * owner);
	void Remove(EnchantmentType * enchantment);
	void Clear();
	bool Empty() const;

public: // hooks
	void TurnEnd();

private:
	inline void BeforeRemove(ItemType & item);

	container_type enchantments;

private:
	Target & target;
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

	template <typename Target> struct hash<GameEngine::Enchantments<Target> > {
		typedef GameEngine::Enchantments<Target> argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const;
	};
}