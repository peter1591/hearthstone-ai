#pragma once

#include <list>
#include <algorithm>

namespace GameEngine {
namespace BoardObjects {

class Minion;

class Enchantment
{
	friend std::hash<Enchantment>;

public:
	enum UniqueIdForHash {
		TypeAttackHPBoost
	};

public:
	Enchantment() {}
	virtual ~Enchantment() {}

	bool operator==(Enchantment const& rhs) const { return this->EqualsTo(rhs); }
	bool operator!=(Enchantment const& rhs) const { return !(*this == rhs); }

public: // hooks
	virtual void AfterAdded(MinionManipulator const& minion) {}
	virtual void BeforeRemoved(MinionManipulator const& minion) {}

	// return false if enchant vanished
	virtual bool TurnEnd(MinionManipulator const& minion) { return true; }

protected:
	virtual bool EqualsTo(Enchantment const& rhs) const = 0; // this is a pure virtual class (i.e., no member to be compared)
	virtual std::size_t GetHash() const = 0; // this is a pure virtual class (i.e., no member to be hashed)
};

// Introduce some attack/hp/taunt/charge/etc. buffs on minion
// buff_flags are ORed flags for MinionStat::Flag
template <int attack_boost, int hp_boost, int buff_flags, bool one_turn>
class Enchantment_BuffStat : public Enchantment
{
public:
	Enchantment_BuffStat()
	{
#ifdef DEBUG
		this->after_added_called = false;
#endif
	}

	void AfterAdded(MinionManipulator const& minion)
	{
#ifdef DEBUG
		this->after_added_called = true;
#endif

		// attack cannot below to zero
		if (minion.GetAttack() + attack_boost < 0)
		{
			this->actual_attack_boost = -minion.GetAttack();
		}
		else {
			this->actual_attack_boost = attack_boost;
		}

		if (this->actual_attack_boost != 0)
		{
			minion.AddAttack(this->actual_attack_boost);
		}

		if (hp_boost != 0)
		{
			minion.IncreaseCurrentAndMaxHP(hp_boost);
		}

		this->SetMinionFlags(minion, true);
	}

	void BeforeRemoved(MinionManipulator const& minion)
	{
#ifdef DEBUG
		if (this->after_added_called == false) throw std::runtime_error("AfterAdded() should be called before");
#endif

		if (this->actual_attack_boost != 0)
		{
			minion.AddAttack(-this->actual_attack_boost);
		}

		if (hp_boost != 0)
		{
			minion.DecreaseMaxHP(hp_boost);
		}

		this->SetMinionFlags(minion, false);
	}

	bool TurnEnd(MinionManipulator const& minion)
	{
		if (one_turn) return false; // one-turn effect 
		else return true;
	}

	bool EqualsTo(Enchantment const& rhs_base) const
	{
		auto rhs = dynamic_cast<decltype(this)>(&rhs_base);
		if (!rhs) return false;

		if (this->actual_attack_boost != rhs->actual_attack_boost) return false;

		return true;
	}

	std::size_t GetHash() const
	{
		std::size_t result = std::hash<int>()(Enchantment::UniqueIdForHash::TypeAttackHPBoost);

		GameEngine::hash_combine(result, this->actual_attack_boost);
		GameEngine::hash_combine(result, hp_boost);
		GameEngine::hash_combine(result, one_turn);

		return result;
	}

private:
	void SetMinionFlags(MinionManipulator const& minion, bool val)
	{
		// TOOD: can use template programming to enforce compile-time loop unrolling
		for (int stat_flag = 0; stat_flag < MinionStat::FLAG_MAX; stat_flag++) {
			int shifted_stat_flag = 1 << stat_flag;

			if ((buff_flags & shifted_stat_flag) == 0) continue; // no buff on this flag

			minion.SetMinionStatFlag((MinionStat::Flag)stat_flag, val);
		}
	}

private:
#ifdef DEBUG
	bool after_added_called;
#endif
	int actual_attack_boost; // attack cannot be negative
};

} // namespace BoardObjects
} // namespace GameEngine

namespace std {
	template <> struct hash<GameEngine::BoardObjects::Enchantment> {
		typedef GameEngine::BoardObjects::Enchantment argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			return s.GetHash();
		}
	};
}