#pragma once

#include <memory>

#include "game-engine/managed-list/list.h"
#include "enchantments-item-type.h"

namespace GameEngine {

class MinionData;
template <typename Target> class Enchantment;
template <typename Target> class ManagedEnchantment;
template <typename EnchantmentTarget> class EnchantmentOwner;
class Minion;

// Maintains the lifetime of Enchantment
template <typename Target>
class Enchantments
{
	friend std::hash<Enchantments>;

public:
	typedef Enchantment<Target> EnchantmentType;
	typedef EnchantmentsItemType<Target> ItemType;
	typedef typename ManagedList<ItemType> ManagedContainer;
	typedef typename ManagedContainer::ManagedItem ManagedItem;

public:
	Enchantments(Target & target) : target(target) {}

	Enchantments(Enchantments<Target> const& rhs) = delete;
	Enchantments<Target> operator=(Enchantments<Target> const& rhs) = delete;
	Enchantments(Target & target, Enchantments<Target> && rhs) = delete;
	Enchantments<Target> & operator=(Enchantments<Target> && rhs) = delete;

	bool operator==(Enchantments const& rhs) const;
	bool operator!=(Enchantments const& rhs) const;

	void Add(std::unique_ptr<EnchantmentType> && enchantment, EnchantmentOwner<Target> * owner);
	void Remove(ManagedEnchantment<Target> & item); // interface for enchantment owner
	void Clear();
	bool Empty() const;

public: // hooks
	void TurnEnd();

private:
	inline void BeforeRemove(ManagedItem item);

	ManagedContainer enchantments;

private:
	Target & target;
};

} // namespace GameEngine

namespace std {
	template <typename Target> struct hash<GameEngine::Enchantments<Target> > {
		typedef GameEngine::Enchantments<Target> argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const;
	};
}