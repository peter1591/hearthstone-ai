#pragma once

#include <list>
#include <memory>
#include <functional>
#include <bitset>
#include "game-engine/card.h"
#include "game-engine/slot-index.h"
#include "object-base.h"
#include "effects.h"
#include "minion-stat.h"

namespace GameEngine {

class Board;

namespace BoardObjects {

class Minion : public ObjectBase
{
	friend std::hash<Minion>;

	public:
		typedef std::function<void(Board& board, SlotIndex idx)> OnDeathTrigger;

	public:
		Minion();

		int GetCardId() const { return this->card_id;}
		int GetAttack() const;
		int GetHP() const;
		int GetMaxHP() const;

		// Initializer
		void Set(int card_id, int origin_attack, int origin_hp, int origin_max_hp);
		void Summon(const Card &card);

		// Modifiers
		void AddAttackThisTurn(int attack);
		void AttackedOnce();
		void TakeDamage(int damage);
		void SetTaunt(bool val) { this->stat.SetFlag(MinionStat::FLAG_TAUNT, val); }
		void SetCharge(bool val) { this->stat.SetFlag(MinionStat::FLAG_CHARGE, val); }
		void SetShield(bool val) { this->stat.SetFlag(MinionStat::FLAG_SHIELD, val); }
		void SetStealth(bool val) { this->stat.SetFlag(MinionStat::FLAG_STEALTH, val); }
		void SetForgetful(bool val) { this->stat.SetFlag(MinionStat::FLAG_FORGETFUL, val); }
		void SetFreezeAttacker(bool val) { this->stat.SetFlag(MinionStat::FLAG_FREEZE_ATTACKER, val); }
		void SetFreezed(bool val) { this->stat.SetFlag(MinionStat::FLAG_FREEZED, val); }

		// Getters
		bool Attackable() const;
		bool IsTaunt() const { return this->stat.GetFlag(MinionStat::FLAG_TAUNT); }
		bool IsCharge() const { return this->stat.GetFlag(MinionStat::FLAG_CHARGE); }
		bool IsShield() const { return this->stat.GetFlag(MinionStat::FLAG_SHIELD); }
		bool IsStealth() const { return this->stat.GetFlag(MinionStat::FLAG_STEALTH); }
		bool IsForgetful() const { return this->stat.GetFlag(MinionStat::FLAG_FORGETFUL); }
		bool IsFreezeAttacker() const { return this->stat.GetFlag(MinionStat::FLAG_FREEZE_ATTACKER); }
		bool IsFreezed() const { return this->stat.GetFlag(MinionStat::FLAG_FREEZED); }

		// Triggers
		void AddOnDeathTrigger(OnDeathTrigger func) { this->triggers_on_death.push_back(func); }
		std::list<OnDeathTrigger> && MoveOutOnDeathTriggers() { return std::move(this->triggers_on_death); }

		// Hooks
		void TurnStart(bool owner_turn);
		void TurnEnd(bool owner_turn);

		bool IsValid() const { return this->card_id != 0; }

		bool operator==(const Minion &rhs) const;
		bool operator!=(const Minion &rhs) const;

	public:
		std::string GetDebugString() const;

	private:
		int card_id;

		// used when being silenced
		int origin_attack;
		int origin_hp;
		int origin_max_hp;

		// current stat (including non-removable effects; excluding aura)
		MinionStat stat;

		// enchantments
		int attack_bias_when_turn_ends;

		int attacked_times;
		bool summoned_this_turn;

		std::list<OnDeathTrigger> triggers_on_death;

		Effects effects;
};

inline Minion::Minion() : card_id(0)
{

}

inline int Minion::GetAttack() const
{
	// TODO: find a timing to update the effects, and cache the updated stat
	MinionStat final_stat = this->stat;
	this->effects.UpdateStat(final_stat);
	return final_stat.GetAttack();
}

inline int Minion::GetHP() const
{
	// TODO: find a timing to update the effects, and cache the updated stat
	MinionStat final_stat = this->stat;
	this->effects.UpdateStat(final_stat);
	return final_stat.GetHP();
}

inline int Minion::GetMaxHP() const
{
	// TODO: find a timing to update the effects, and cache the updated stat
	MinionStat final_stat = this->stat;
	this->effects.UpdateStat(final_stat);
	return final_stat.GetMaxHP();
}

inline void Minion::Set(int card_id, int origin_attack, int origin_hp, int origin_max_hp)
{
	this->card_id = card_id;
	this->origin_attack = origin_attack;
	this->origin_hp = origin_hp;
	this->origin_max_hp = origin_max_hp;

	this->stat.SetAttack(this->origin_attack);
	this->stat.SetHP(this->origin_hp);
	this->stat.SetMaxHP(this->origin_max_hp);
	this->stat.ClearFlags();

	this->attack_bias_when_turn_ends = 0;
}

inline void Minion::Summon(const Card & card)
{
	this->card_id = card.id;
	this->origin_max_hp = card.data.minion.hp;
	this->origin_hp = this->origin_max_hp;
	this->origin_attack = card.data.minion.attack;

	this->stat.SetAttack(this->origin_attack);
	this->stat.SetHP(this->origin_hp);
	this->stat.SetMaxHP(this->origin_max_hp);
	this->SetTaunt(card.data.minion.taunt);
	this->SetCharge(card.data.minion.charge);
	this->SetShield(card.data.minion.shield);
	this->SetStealth(card.data.minion.stealth);
	this->SetForgetful(card.data.minion.forgetful);
	this->SetFreezeAttacker(card.data.minion.freeze);
	this->SetFreezed(false);

	this->attacked_times = 0;
	this->summoned_this_turn = true;
	this->attack_bias_when_turn_ends = 0;
}

inline void Minion::AddAttackThisTurn(int attack_boost)
{
	int current_attack = this->stat.GetAttack();
	if (current_attack + attack_boost <= 0) {
		// cannot reduced below zero
		attack_boost = -current_attack;
	}
	this->stat.SetAttack(current_attack + attack_boost);
	this->attack_bias_when_turn_ends -= attack_boost;
}

inline void Minion::TurnStart(bool)
{
	this->summoned_this_turn = false;
	this->attacked_times = 0;
}

inline void Minion::TurnEnd(bool owner_turn)
{
	this->stat.SetAttack(this->stat.GetAttack() + this->attack_bias_when_turn_ends);
	this->attack_bias_when_turn_ends = 0;

	if (owner_turn) {
		// check thaw
		if (this->attacked_times == 0 && !this->summoned_this_turn)
		{
			// if summon in this turn, and freeze it, then the minion will not be unfrozen
			this->stat.SetFlag(MinionStat::FLAG_FREEZED, false);
		}
	}
}

inline bool Minion::Attackable() const
{
	if (this->summoned_this_turn && !this->stat.GetFlag(MinionStat::FLAG_CHARGE)) return false;

	if (this->stat.GetFlag(MinionStat::FLAG_FREEZED)) return false;

	if (attacked_times > 0) return false;
	if (this->GetAttack() <= 0) return false;

	return true;
}

inline void Minion::AttackedOnce()
{
	this->attacked_times++;
	this->stat.SetFlag(MinionStat::FLAG_STEALTH, false);
}

inline void Minion::TakeDamage(int damage)
{
	if (this->stat.GetFlag(MinionStat::FLAG_SHIELD)) {
		this->stat.SetFlag(MinionStat::FLAG_SHIELD, false);
	}
	else {
		this->stat.SetHP(this->stat.GetHP() - damage);
	}
}

inline bool Minion::operator==(Minion const& rhs) const
{
	if (this->card_id != rhs.card_id) return false;
	if (this->origin_attack != rhs.origin_attack) return false;
	if (this->origin_hp != rhs.origin_hp) return false;
	if (this->origin_max_hp != rhs.origin_max_hp) return false;

	if (this->stat != rhs.stat) return false;

	if (this->attack_bias_when_turn_ends != rhs.attack_bias_when_turn_ends) return false;
	if (this->attacked_times != rhs.attacked_times) return false;
	if (this->summoned_this_turn != rhs.summoned_this_turn) return false;

	if (this->effects != rhs.effects) return false;

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
		oss << "[" << this->GetCardId() << "] " << this->GetAttack() << " / " << this->GetHP() << " (max hp = " << this->GetMaxHP() << ")";

		if (this->IsTaunt()) oss << " [TAUNT]";
		if (this->IsCharge()) oss << " [CHARGE]";
		if (this->IsShield()) oss << " [SHIELD]";
		if (this->IsStealth()) oss << " [STEALTH]";
		if (this->IsForgetful()) oss << " [FORGETFUL]";
		if (this->IsFreezeAttacker()) oss << " [FREEZE]";
		if (this->IsFreezed()) oss << " [FREEZED]";
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
			GameEngine::hash_combine(result, s.origin_attack);
			GameEngine::hash_combine(result, s.origin_hp);
			GameEngine::hash_combine(result, s.origin_max_hp);
			
			GameEngine::hash_combine(result, s.stat);
			
			GameEngine::hash_combine(result, s.attack_bias_when_turn_ends);
			GameEngine::hash_combine(result, s.attacked_times);
			GameEngine::hash_combine(result, s.summoned_this_turn);

			GameEngine::hash_combine(result, s.effects);

			return result;
		}
	};
}