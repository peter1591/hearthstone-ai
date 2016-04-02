#pragma once

#include <list>
#include <map>
#include <memory>

#include "managed-enchantment.h"

namespace GameEngine {

class MinionData;
template <typename Target> class Enchantment;
class EnchantmentOwner;
class Minion;

// Maintains the lifetime of Enchantment
template <typename Target>
class Enchantments
{
	friend std::hash<Enchantments>;

private:
	typedef Enchantment<Target> EnchantmentType;
	typedef ManagedEnchantment<Target> ManagedEnchantmentType;
	typedef typename std::list<std::pair<ManagedEnchantmentType, EnchantmentOwner*>> container_type;

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
	typename container_type::iterator Remove(typename container_type::iterator it);
	container_type enchantments;

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