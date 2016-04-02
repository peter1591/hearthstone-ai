#pragma once

#include <list>
#include <algorithm>

namespace GameEngine {

class MinionData;
class Minion;

template <typename Target>
class Enchantment
{
	friend std::hash<Enchantment>;

public:
	enum UniqueIdForHash {
		TypeBuffMinion,
		TypeBuffMinion_C
	};

public:
	Enchantment();
	virtual ~Enchantment();

	bool operator==(Enchantment<Target> const& rhs) const;
	bool operator!=(Enchantment<Target> const& rhs) const;

public: // hooks
	virtual void AfterAdded(Target & minion) {}
	virtual void BeforeRemoved(Target & minion) {}

	// return false if enchant vanished
	// TODO: modify this; use return value in this way might be bad...
	virtual bool TurnEnd(Target & minion) { return true; }

protected:
	virtual bool EqualsTo(Enchantment<Target> const& rhs) const = 0; // this is a pure virtual class (i.e., no member to be compared)
	virtual std::size_t GetHash() const = 0; // this is a pure virtual class (i.e., no member to be hashed)
};

} // namespace GameEngine

namespace std {
	template <typename Target> struct hash<GameEngine::Enchantment<Target> > {
		typedef GameEngine::Enchantment<Target> argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			return s.GetHash();
		}
	};
}

#include "enchantment-impl.h"