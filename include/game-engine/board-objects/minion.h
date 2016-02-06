#pragma once

#include <list>
#include <memory>
#include <functional>
#include "game-engine/card.h"
#include "game-engine/slot-index.h"
#include "object-base.h"

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
		int GetAttack() const { return this->attack; }
		int GetHP() const { return this->hp; }
		int GetMaxHP() const {return this->max_hp; }

		// Initializer
		void Set(int card_id, int origin_attack, int origin_hp, int origin_max_hp);
		void Summon(const Card &card);

		// Modifiers
		void AddAttackThisTurn(int attack);
		void AttackedOnce();
		void TakeDamage(int damage);
		void SetTaunt(bool taunt) { this->taunt = taunt; }
		void SetCharge(bool charge) { this->charge = charge; }
		void SetShield(bool shield) { this->shield = shield; }
		void SetStealth(bool stealth) { this->stealth = stealth; }
		void SetForgetful(bool forgetful) { this->forgetful = forgetful; }
		void SetFreezeAttacker(bool freeze) { this->freeze_attacker = freeze; }
		void SetFreezed(bool freezed) { this->freezed = freezed; }

		// Getters
		bool Attackable() const;
		bool IsTaunt() const { return this->taunt; }
		bool IsCharge() const { return this->charge; }
		bool IsShield() const { return this->shield; }
		bool IsStealth() const { return this->stealth; }
		bool IsForgetful() const { return this->forgetful; }
		bool IsFreezeAttacker() const { return this->freeze_attacker; }
		bool IsFreezed() const { return this->freezed; }

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

		// used when silenced
		int origin_attack;
		int origin_hp;
		int origin_max_hp;

		// current stat (exclude aura)
		int attack;
		int hp;
		int max_hp;
		bool taunt;
		bool charge;
		bool shield;
		bool stealth;
		bool forgetful;
		bool freeze_attacker;
		bool freezed;

		// enchantments
		int attack_bias_when_turn_ends;

		int attacked_times;
		bool summoned_this_turn;

		std::list<OnDeathTrigger> triggers_on_death;
};

inline Minion::Minion() : card_id(0)
{

}

inline void Minion::Set(int card_id, int origin_attack, int origin_hp, int origin_max_hp)
{
	this->card_id = card_id;
	this->origin_attack = origin_attack;
	this->origin_hp = origin_hp;
	this->origin_max_hp = origin_max_hp;

	this->attack = this->origin_attack;
	this->hp = this->origin_hp;
	this->max_hp = this->origin_max_hp;
	this->taunt = false;
	this->charge = false;
	this->shield = false;
	this->stealth = false;
	this->forgetful = false;
	this->freeze_attacker = false;
	this->freezed = false;

	this->attack_bias_when_turn_ends = 0;
}

inline void Minion::Summon(const Card & card)
{
	this->card_id = card.id;
	this->origin_max_hp = card.data.minion.hp;
	this->origin_hp = this->origin_max_hp;
	this->origin_attack = card.data.minion.attack;

	this->attack = this->origin_attack;
	this->hp = this->origin_hp;
	this->max_hp = this->origin_max_hp;
	this->taunt = card.data.minion.taunt;
	this->charge = card.data.minion.charge;
	this->shield = card.data.minion.shield;
	this->stealth = card.data.minion.stealth;
	this->forgetful = card.data.minion.forgetful;
	this->freeze_attacker = card.data.minion.freeze;
	this->freezed = false;

	this->attacked_times = 0;
	this->summoned_this_turn = true;
	this->attack_bias_when_turn_ends = 0;
}

inline void Minion::AddAttackThisTurn(int attack)
{
	if (this->attack + attack <= 0) {
		// cannot reduced below zero
		attack = -this->attack;
	}
	this->attack += attack;
	this->attack_bias_when_turn_ends -= attack;
}

inline void Minion::TurnStart(bool)
{
	this->summoned_this_turn = false;
	this->attacked_times = 0;
}

inline void Minion::TurnEnd(bool owner_turn)
{
	this->attack += this->attack_bias_when_turn_ends;
	this->attack_bias_when_turn_ends = 0;

	if (owner_turn) {
		// check thaw
		if (this->attacked_times == 0 && !this->summoned_this_turn)
		{
			// if summon in this turn, and freeze it, then the minion will not be unfrozen
			this->freezed = false;
		}
	}
}

inline bool Minion::Attackable() const
{
	if (this->summoned_this_turn && !this->charge) return false;

	if (this->freezed) return false;

	if (attacked_times > 0) return false;
	if (this->GetAttack() <= 0) return false;

	return true;
}

inline void Minion::AttackedOnce()
{
	this->attacked_times++;
	this->stealth = false;
}

inline void Minion::TakeDamage(int damage)
{
	if (this->shield) {
		this->shield = false;
	}
	else {
		this->hp -= damage;
	}
}

inline bool Minion::operator==(Minion const& rhs) const
{
	if (this->card_id != rhs.card_id) return false;
	if (this->origin_attack != rhs.origin_attack) return false;
	if (this->origin_hp != rhs.origin_hp) return false;
	if (this->origin_max_hp != rhs.origin_max_hp) return false;

	if (this->attack != rhs.attack) return false;
	if (this->hp != rhs.hp) return false;
	if (this->max_hp != rhs.max_hp) return false;
	if (this->taunt != rhs.taunt) return false;
	if (this->charge != rhs.charge) return false;
	if (this->shield != rhs.shield) return false;
	if (this->stealth != rhs.stealth) return false;
	if (this->forgetful != rhs.forgetful) return false;
	if (this->freeze_attacker != rhs.freeze_attacker) return false;
	if (this->freezed != rhs.freezed) return false;

	if (this->attack_bias_when_turn_ends != rhs.attack_bias_when_turn_ends) return false;
	if (this->attacked_times != rhs.attacked_times) return false;
	if (this->summoned_this_turn != rhs.summoned_this_turn) return false;

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
			GameEngine::hash_combine(result, s.taunt);
			GameEngine::hash_combine(result, s.charge);
			GameEngine::hash_combine(result, s.shield);
			GameEngine::hash_combine(result, s.stealth);
			GameEngine::hash_combine(result, s.forgetful);
			GameEngine::hash_combine(result, s.freeze_attacker);
			GameEngine::hash_combine(result, s.freezed);

			GameEngine::hash_combine(result, s.attack);
			GameEngine::hash_combine(result, s.hp);
			GameEngine::hash_combine(result, s.max_hp);
			GameEngine::hash_combine(result, s.attack_bias_when_turn_ends);
			GameEngine::hash_combine(result, s.attacked_times);
			GameEngine::hash_combine(result, s.summoned_this_turn);

			return result;
		}
	};
}

