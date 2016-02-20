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
		TypeBuffMinion
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
template <int attack_boost, int hp_boost, int buff_flags, bool one_turn>
class Enchantment_BuffMinion : public Enchantment<Minion>
{
public:
	Enchantment_BuffMinion()
	{
#ifdef DEBUG
		this->after_added_called = false;
#endif
	}

	void AfterAdded(Minion & minion)
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

	void BeforeRemoved(Minion & minion)
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

	bool TurnEnd(Minion & minion)
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
		std::size_t result = std::hash<int>()(Enchantment::UniqueIdForHash::TypeBuffMinion);

		GameEngine::hash_combine(result, this->actual_attack_boost);
		GameEngine::hash_combine(result, hp_boost);
		GameEngine::hash_combine(result, buff_flags);
		GameEngine::hash_combine(result, one_turn);

		return result;
	}

private:
	template <int alternating_flag>
	class MinionFlagsSetter {
	public:
		static void SetFlag(Minion & minion, bool val) {
			constexpr int shifted_flag = 1 << alternating_flag;
			constexpr bool alternating = (buff_flags & shifted_flag) != 0;

			if (alternating) {
				minion.SetMinionStatFlag((MinionStat::Flag)alternating_flag, val);
			}

			MinionFlagsSetter<alternating_flag - 1>::SetFlag(minion, val);
		}
	};

	template <>
	class MinionFlagsSetter<0> {
	public:
		static void SetFlag(Minion & minion, bool val) {
			return;
		}
	};

	void SetMinionFlags(Minion & minion, bool val)
	{
		MinionFlagsSetter<MinionStat::FLAG_MAX - 1>::SetFlag(minion, val);
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
	template <typename Target> struct hash<GameEngine::BoardObjects::Enchantment<Target> > {
		typedef GameEngine::BoardObjects::Enchantment<Target> argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			return s.GetHash();
		}
	};
}