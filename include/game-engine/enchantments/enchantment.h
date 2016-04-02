#pragma once

#include <list>
#include <algorithm>

namespace GameEngine {
namespace BoardObjects {

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
	Enchantment() {}
	virtual ~Enchantment() {}

	bool operator==(Enchantment<Target> const& rhs) const { return this->EqualsTo(rhs); }
	bool operator!=(Enchantment<Target> const& rhs) const { return !(*this == rhs); }

public: // hooks
	virtual void AfterAdded(Target & minion) {}
	virtual void BeforeRemoved(Target & minion) {}

	// return false if enchant vanished
	virtual bool TurnEnd(Target & minion) { return true; }

protected:
	virtual bool EqualsTo(Enchantment<Target> const& rhs) const = 0; // this is a pure virtual class (i.e., no member to be compared)
	virtual std::size_t GetHash() const = 0; // this is a pure virtual class (i.e., no member to be hashed)
};

// Introduce some attack/hp/taunt/charge/etc. buffs on minion
// buff_flags are ORed flags for MinionStat::Flag
class Enchantment_BuffMinion : public Enchantment<Minion>
{
public:
	Enchantment_BuffMinion(int attack_boost, int hp_boost, int buff_flags, bool one_turn);
	bool EqualsTo(Enchantment<Minion> const& rhs_base) const;
	std::size_t GetHash() const;

public:
	void AfterAdded(Minion & minion);
	void BeforeRemoved(Minion & minion);
	bool TurnEnd(Minion & minion);

private:
#ifdef DEBUG
	bool after_added_called;
#endif
	int attack_boost;
	int hp_boost;
	int buff_flags;
	bool one_turn;
	int actual_attack_boost; // attack cannot be negative
};

// Introduce some attack/hp/taunt/charge/etc. buffs on minion
// buff_flags are ORed flags for MinionStat::Flag
template <int attack_boost, int hp_boost, int buff_flags, bool one_turn>
class Enchantment_BuffMinion_C : public Enchantment<Minion>
{
public:
	Enchantment_BuffMinion_C();
	bool EqualsTo(Enchantment<Minion> const& rhs_base) const;
	std::size_t GetHash() const;

public:
	void AfterAdded(Minion & minion);
	void BeforeRemoved(Minion & minion);
	bool TurnEnd(Minion & minion);

private:
#ifdef DEBUG
	bool after_added_called;
#endif
	int actual_attack_boost; // attack cannot be negative
};

} // namespace BoardObjects
} // namespace GameEngine

namespace std {
	template <typename Target> struct hash<GameEngine::BoardObjects::Enchantment<Target> > {
		typedef GameEngine::BoardObjects::Enchantment<Target> argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			return s.GetHash();
		}
	};
}

#include "enchantment-impl.h"