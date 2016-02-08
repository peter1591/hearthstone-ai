#pragma once

#include <list>
#include <cmath>

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
	virtual void AfterAdded(Minion * minion) {}

	virtual void BeforeRemoved(Minion * minion) {}

	// return false if enchant vanished
	virtual bool TurnEnd(Minion * minion) { return true; }

protected:
	virtual bool EqualsTo(Enchantment const& rhs) const = 0; // this is a pure virtual class (i.e., no member to be compared)
	virtual std::size_t GetHash() const = 0; // this is a pure virtual class (i.e., no member to be hashed)
};

template <int attack_boost, int hp_boost, bool one_turn>
class Enchantment_AttackHPBoost : public Enchantment
{
public:
	void AfterAdded(Minion * minion)
	{
		if (attack_boost != 0)
		{
			minion->stat.SetAttack(minion->stat.GetAttack() + attack_boost);
		}

		if (hp_boost != 0)
		{
			minion->stat.SetMaxHP(minion->stat.GetMaxHP() + hp_boost);
			minion->stat.SetHP(minion->stat.GetHP() + hp_boost);
		}
	}

	void BeforeRemoved(Minion * minion)
	{
		if (attack_boost != 0)
		{
			minion->stat.SetAttack(minion->stat.GetAttack() - attack_boost);
		}

		if (hp_boost != 0)
		{
			minion->stat.SetMaxHP(minion->stat.GetMaxHP() - hp_boost);
			minion->stat.SetHP(
				std::min(minion->stat.GetHP(), minion->stat.GetMaxHP()));
		}
	}

	bool EqualsTo(Enchantment const& rhs_base) const
	{
		auto rhs = dynamic_cast<decltype(this)>(&rhs_base);
		if (!rhs) return false;

		return true;
	}

	bool TurnEnd(Minion *)
	{
		if (one_turn) return false; // one-turn effect 
		else return true;
	}

	std::size_t GetHash() const
	{
		std::size_t result = std::hash<int>()(Enchantment::UniqueIdForHash::TypeAttackHPBoost);

		GameEngine::hash_combine(result, attack_boost);
		GameEngine::hash_combine(result, hp_boost);
		GameEngine::hash_combine(result, one_turn);

		return result;
	}
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