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
#include "game-engine/on-death-trigger.h"

namespace GameEngine {

class Board;

namespace BoardObjects {

class Minion;
class MinionIterator;

class MinionData
{
	friend std::hash<MinionData>;

public:
	typedef GameEngine::OnDeathTrigger<MinionIterator &> OnDeathTrigger;

public:
	MinionData();

	MinionData(MinionData const& rhs) {
		// If minion has enchantments/auras, then it cannot be cloned
		// Note: The only chance we need to copy minion is to copy root node board in MCTS
		// If root node board has enchantments/auras, then ask the caller to prepare the root node board again
		if (!this->enchantments.Empty()) throw std::runtime_error("You should not copy minion with enchantments");
		if (!this->auras.Empty()) throw std::runtime_error("You should not copy minion with auras");

		this->card_id = rhs.card_id;
		this->stat = rhs.stat;
		this->attacked_times = rhs.attacked_times;
		this->summoned_this_turn = rhs.summoned_this_turn;
		this->pending_removal = rhs.pending_removal;
		this->triggers_on_death = rhs.triggers_on_death;
	}
	MinionData & operator=(MinionData const& rhs) = delete;

	MinionData(MinionData && rhs) {
		this->card_id = std::move(rhs.card_id);
		this->stat = std::move(rhs.stat);
		this->attacked_times = std::move(rhs.attacked_times);
		this->summoned_this_turn = std::move(rhs.summoned_this_turn);
		this->pending_removal = std::move(rhs.pending_removal);
		this->triggers_on_death = std::move(rhs.triggers_on_death);
		this->enchantments = std::move(rhs.enchantments);
		this->auras = std::move(rhs.auras);
	}
	MinionData & operator=(MinionData && rhs) = delete;

	bool operator==(const MinionData &rhs) const;
	bool operator!=(const MinionData &rhs) const;

	// Initializer
	void Set(int card_id, int attack, int hp, int max_hp);
	void Summon(const Card &card);

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

	Enchantments<Minion> enchantments;
	Auras auras; // owned auras
};

inline MinionData::MinionData() : card_id(0), pending_removal(false)
{

}

inline void MinionData::Set(int card_id, int attack, int hp, int max_hp)
{
	this->card_id = card_id;

	this->stat.SetAttack(attack);
	this->stat.SetHP(hp);
	this->stat.SetMaxHP(max_hp);
}

inline void MinionData::Summon(const Card & card)
{
	this->card_id = card.id;

	this->stat.SetAttack(card.data.minion.attack);
	this->stat.SetHP(card.data.minion.hp);
	this->stat.SetMaxHP(card.data.minion.hp);

	if (card.data.minion.taunt) this->stat.SetTaunt();
	if (card.data.minion.charge) this->stat.SetCharge();
	if (card.data.minion.shield) this->stat.SetShield();
	if (card.data.minion.stealth) this->stat.SetStealth();
	if (card.data.minion.forgetful) this->stat.SetForgetful();
	if (card.data.minion.freeze) this->stat.SetFreezeAttacker();
	if (card.data.minion.windfury) this->stat.SetWindFury();
	if (card.data.minion.poisonous) this->stat.SetPoisonous();

	this->attacked_times = 0;
	this->summoned_this_turn = true;
}

inline bool MinionData::operator==(MinionData const& rhs) const
{
	if (this->card_id != rhs.card_id) return false;

	if (this->stat != rhs.stat) return false;

	if (this->attacked_times != rhs.attacked_times) return false;
	if (this->summoned_this_turn != rhs.summoned_this_turn) return false;

	if (this->pending_removal != rhs.pending_removal) return false;

	if (this->triggers_on_death != rhs.triggers_on_death) return false;

	if (this->enchantments != rhs.enchantments) return false;
	if (this->auras != rhs.auras) return false;

	return true;
}

inline bool MinionData::operator!=(MinionData const& rhs) const
{
	return !(*this == rhs);
}

inline std::string MinionData::GetDebugString() const
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
	template <> struct hash<GameEngine::BoardObjects::MinionData> {
		typedef GameEngine::BoardObjects::MinionData argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			GameEngine::hash_combine(result, s.card_id);
			
			GameEngine::hash_combine(result, s.stat);
			
			GameEngine::hash_combine(result, s.attacked_times);
			GameEngine::hash_combine(result, s.summoned_this_turn);

			GameEngine::hash_combine(result, s.pending_removal);

			for (auto const& trigger : s.triggers_on_death) {
				GameEngine::hash_combine(result, trigger);
			}

			GameEngine::hash_combine(result, s.enchantments);
			GameEngine::hash_combine(result, s.auras);

			return result;
		}
	};
}