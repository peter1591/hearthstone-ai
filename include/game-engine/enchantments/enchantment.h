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
	Enchantment();
	virtual ~Enchantment();

	bool operator==(Enchantment<Target> const& rhs) const;
	bool operator!=(Enchantment<Target> const& rhs) const;

public: // hooks
	virtual void AfterAdded(Target & target) { (void)target; }
	virtual void BeforeRemoved(Target & target) { (void)target; }

	// return false if enchant vanished
	virtual void TurnEnd(Target & target, bool & expired) { (void)target; expired = false; }

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