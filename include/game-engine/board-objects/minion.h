#pragma once

#include <algorithm>
#include <list>
#include <memory>
#include <functional>
#include <bitset>
#include "game-engine/card.h"
#include "game-engine/slot-index.h"
#include "object-base.h"
#include "minion-stat.h"
#include "enchantments.h"
#include "aura.h"

namespace GameEngine {

class Board;

namespace BoardObjects {

class MinionManipulator;

class Minion
{
	friend std::hash<Minion>;

public:
	typedef std::function<void(MinionManipulator & triggering_minion)> OnDeathTrigger;

public:
	Minion();

	// Shallow copy is allowed to be compliance to STL containers
	// Runtime check for deep-clone since enchantments and auras are put on heap
	void CheckCanBeSafelyCloned() const;

	bool operator==(const Minion &rhs) const;
	bool operator!=(const Minion &rhs) const;

	// Initializer
	void Set(int card_id, int attack, int hp, int max_hp);
	void Summon(const Card &card);

	// Getters
	bool Attackable() const;

	bool IsValid() const { return this->card_id != 0; }

public:
	std::string GetDebugString() const;

public:
	int card_id;

	MinionStat stat;

	int attacked_times;
	bool summoned_this_turn;

	// mark as pending death when triggering deathrattles
	bool pending_removal;

	std::list<OnDeathTrigger> triggers_on_death;

	Enchantments<MinionManipulator> enchantments;
	Auras auras; // owned auras
};

inline Minion::Minion() : card_id(0), pending_removal(false)
{

}

inline void Minion::Set(int card_id, int attack, int hp, int max_hp)
{
	this->card_id = card_id;

	this->stat.SetAttack(attack);
	this->stat.SetHP(hp);
	this->stat.SetMaxHP(max_hp);
}

inline void Minion::Summon(const Card & card)
{
	this->card_id = card.id;

	this->stat.SetAttack(card.data.minion.attack);
	this->stat.SetHP(card.data.minion.hp);
	this->stat.SetMaxHP(card.data.minion.hp);

	if (card.data.minion.taunt) this->stat.SetTaunt(true);
	if (card.data.minion.charge) this->stat.SetCharge(true);
	if (card.data.minion.shield) this->stat.SetShield(true);
	if (card.data.minion.stealth) this->stat.SetStealth(true);
	if (card.data.minion.forgetful) this->stat.SetForgetful(true);
	if (card.data.minion.freeze) this->stat.SetFreezeAttacker(true);
	if (card.data.minion.windfury) this->stat.SetWindFury(true);
	if (card.data.minion.poisonous) this->stat.SetPoisonous(true);

	this->attacked_times = 0;
	this->summoned_this_turn = true;
}

inline bool Minion::Attackable() const
{
	if (this->summoned_this_turn && !this->stat.IsCharge()) return false;

	if (this->stat.IsFreezed()) return false;

	int max_attacked_times = 1;
	if (this->stat.IsWindFury()) max_attacked_times = 2;

	if (this->attacked_times >= max_attacked_times) return false;

	if (this->stat.GetAttack() <= 0) return false;

	return true;
}

inline void Minion::CheckCanBeSafelyCloned() const
{
	this->enchantments.CheckCanBeSafelyCloned();
	this->auras.CheckCanBeSafelyCloned();
}

inline bool Minion::operator==(Minion const& rhs) const
{
	if (this->card_id != rhs.card_id) return false;

	if (this->stat != rhs.stat) return false;

	if (this->attacked_times != rhs.attacked_times) return false;
	if (this->summoned_this_turn != rhs.summoned_this_turn) return false;

	if (this->pending_removal != rhs.pending_removal) return false;

	if (this->enchantments != rhs.enchantments) return false;
	if (this->auras != rhs.auras) return false;

	return true;
}

inline bool Minion::operator!=(Minion const& rhs) const
{
	return !(*this == rhs);
}

inline std::string Minion::GetDebugString() const
{
	std::ostringstream oss;

	if (!this->IsValid()) {
		oss << "[EMPTY]";
	}
	else {
		oss << "[" << this->card_id << "] " << this->stat.GetAttack() << " / " << this->stat.GetHP() << " (max hp = " << this->stat.GetMaxHP() << ")";

		if (this->stat.IsTaunt()) oss << " [TAUNT]";
		if (this->stat.IsCharge()) oss << " [CHARGE]";
		if (this->stat.IsShield()) oss << " [SHIELD]";
		if (this->stat.IsStealth()) oss << " [STEALTH]";
		if (this->stat.IsForgetful()) oss << " [FORGETFUL]";
		if (this->stat.IsFreezeAttacker()) oss << " [FREEZE]";
		if (this->stat.IsFreezed()) oss << " [FREEZED]";
		if (this->stat.IsWindFury()) oss << " [WINDFURY]";
		if (this->stat.IsPoisonous()) oss << " [POISONOUS]";
	}

	return oss.str();
}

} // namespace GameEngine
}

namespace std {
	template <> struct hash<GameEngine::BoardObjects::Minion> {
		typedef GameEngine::BoardObjects::Minion argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			GameEngine::hash_combine(result, s.card_id);
			
			GameEngine::hash_combine(result, s.stat);
			
			GameEngine::hash_combine(result, s.attacked_times);
			GameEngine::hash_combine(result, s.summoned_this_turn);

			GameEngine::hash_combine(result, s.pending_removal);

			GameEngine::hash_combine(result, s.enchantments);
			GameEngine::hash_combine(result, s.auras);

			return result;
		}
	};
}