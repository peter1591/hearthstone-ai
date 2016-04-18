#pragma once

#include <memory>

#include "game-engine/managed-list/list.h"
#include "game-engine/enchantments/enchantment.h"
#include "enchantments-item-type.h"

namespace GameEngine {

class MinionData;
template <typename Target> class ManagedEnchantment;
template <typename EnchantmentTarget> class EnchantmentsOwner;
class Minion;
class MinionAura;

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

	// forward declaration for enchantments owner
	using OwnerContainer = std::list<ManagedEnchantment<Target>>;
	using OwnerToken = typename OwnerContainer::iterator;

public:
	Enchantments(Target & target) : target(target) {}
	~Enchantments();

	Enchantments(Enchantments<Target> const& rhs) = delete;
	Enchantments<Target> operator=(Enchantments<Target> const& rhs) = delete;
	Enchantments(Target & target, Enchantments<Target> && rhs) = delete;
	Enchantments<Target> & operator=(Enchantments<Target> && rhs) = delete;

	bool operator==(Enchantments const& rhs) const;
	bool operator!=(Enchantments const& rhs) const;

	// destory all allocated resource without triggering any hooks (since the whole board is going to be destroyed)
	void Destroy();

	void Add(std::unique_ptr<EnchantmentType> && enchantment, MinionAura & aura);
	void Add(std::unique_ptr<EnchantmentType> && enchantment, EnchantmentsOwner<Target> * owner = nullptr);

	void Remove(ManagedEnchantment<Target> & item); // interface for enchantment owner
	void Clear();
	bool Empty() const;

public: // hooks
	void TurnEnd();

private:
	void BeforeRemove(ManagedItem item);

private:
	Target & target;
	ManagedContainer enchantments;
};

} // namespace GameEngine

namespace std {
	template <typename Target> struct hash<GameEngine::Enchantments<Target> > {
		typedef GameEngine::Enchantments<Target> argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const;
	};
}