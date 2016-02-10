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

class MinionsIteratorWithIndex;
class MinionsConstIteratorWithIndex;

class Minion : public ObjectBase
{
	friend std::hash<Minion>;

	public:
		typedef std::function<void(Board& board, MinionsIteratorWithIndex triggering_minion)> OnDeathTrigger;

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
		int GetCardId() const { return this->card_id; }
		int GetAttack() const { return this->stat.GetAttack(); }
		int GetHP() const { return this->stat.GetHP(); }
		int GetMaxHP() const { return this->stat.GetMaxHP(); }
		bool Attackable() const;
		bool IsTaunt() const { return this->stat.GetFlag(MinionStat::FLAG_TAUNT); }
		bool IsCharge() const { return this->stat.GetFlag(MinionStat::FLAG_CHARGE); }
		bool IsShield() const { return this->stat.GetFlag(MinionStat::FLAG_SHIELD); }
		bool IsStealth() const { return this->stat.GetFlag(MinionStat::FLAG_STEALTH); }
		bool IsForgetful() const { return this->stat.GetFlag(MinionStat::FLAG_FORGETFUL); }
		bool IsFreezeAttacker() const { return this->stat.GetFlag(MinionStat::FLAG_FREEZE_ATTACKER); }
		bool IsFreezed() const { return this->stat.GetFlag(MinionStat::FLAG_FREEZED); }

		// Setters
		void AttackedOnce();
		void AddAttack(int val) { this->stat.SetAttack(this->stat.GetAttack() + val); }
		void IncreaseCurrentAndMaxHP(int val);
		void DecreaseMaxHP(int val);
		void TakeDamage(int damage);
		void SetTaunt(bool val) { this->stat.SetFlag(MinionStat::FLAG_TAUNT, val); }
		void SetCharge(bool val) { this->stat.SetFlag(MinionStat::FLAG_CHARGE, val); }
		void SetShield(bool val) { this->stat.SetFlag(MinionStat::FLAG_SHIELD, val); }
		void SetStealth(bool val) { this->stat.SetFlag(MinionStat::FLAG_STEALTH, val); }
		void SetForgetful(bool val) { this->stat.SetFlag(MinionStat::FLAG_FORGETFUL, val); }
		void SetFreezeAttacker(bool val) { this->stat.SetFlag(MinionStat::FLAG_FREEZE_ATTACKER, val); }
		void SetFreezed(bool val) { this->stat.SetFlag(MinionStat::FLAG_FREEZED, val); }

		// Triggers
		void AddOnDeathTrigger(OnDeathTrigger func) { this->triggers_on_death.push_back(func); }
		std::list<OnDeathTrigger> && MoveOutOnDeathTriggers() { return std::move(this->triggers_on_death); }

		// Enchantment
		void AddEnchantment(Enchantment * enchantment, EnchantmentOwner * owner) { this->enchantments.Add(enchantment, this, owner); }
		void RemoveEnchantment(Enchantment * enchantment) { this->enchantments.Remove(enchantment, this); }
		void ClearEnchantments() { this->enchantments.Clear(this); }

		// Aura
		void AddAura(GameEngine::Board & board, MinionsIteratorWithIndex &myself, Aura * aura) { this->auras.Add(board, myself, aura); }
		void ClearAuras(GameEngine::Board & board, MinionsIteratorWithIndex &myself) { this->auras.Clear(board, myself); }

		// Hooks
		void TurnStart(bool owner_turn);
		void TurnEnd(bool owner_turn);
		void HookAfterMinionAdded(Board & board, MinionsIteratorWithIndex & myself, MinionsIteratorWithIndex & added_minion);
		void HookMinionCheckEnraged(Board & board, MinionsIteratorWithIndex & myself);

		bool IsValid() const { return this->card_id != 0; }

	public:
		std::string GetDebugString() const;

	private:
		int card_id;

		MinionStat stat;

		int attacked_times;
		bool summoned_this_turn;

		std::list<OnDeathTrigger> triggers_on_death;

		Enchantments enchantments;
		Auras auras; // owned auras
};

inline Minion::Minion() : card_id(0)
{

}

inline void Minion::Set(int card_id, int attack, int hp, int max_hp)
{
	this->card_id = card_id;

	this->stat.SetAttack(attack);
	this->stat.SetHP(hp);
	this->stat.SetMaxHP(max_hp);
	this->stat.ClearFlags();
}

inline void Minion::Summon(const Card & card)
{
	this->card_id = card.id;

	this->stat.SetAttack(card.data.minion.attack);
	this->stat.SetHP(card.data.minion.hp);
	this->stat.SetMaxHP(card.data.minion.hp);
	this->SetTaunt(card.data.minion.taunt);
	this->SetCharge(card.data.minion.charge);
	this->SetShield(card.data.minion.shield);
	this->SetStealth(card.data.minion.stealth);
	this->SetForgetful(card.data.minion.forgetful);
	this->SetFreezeAttacker(card.data.minion.freeze);
	this->SetFreezed(false);

	this->attacked_times = 0;
	this->summoned_this_turn = true;
}

inline void Minion::TurnStart(bool)
{
	this->summoned_this_turn = false;
	this->attacked_times = 0;
}

inline void Minion::TurnEnd(bool owner_turn)
{
	if (owner_turn) {
		// check thaw
		if (this->attacked_times == 0 && !this->summoned_this_turn)
		{
			// if summon in this turn, and freeze it, then the minion will not be unfrozen
			this->stat.SetFlag(MinionStat::FLAG_FREEZED, false);
		}
	}

	this->enchantments.TurnEnd(this);
}

inline void Minion::HookAfterMinionAdded(Board & board, MinionsIteratorWithIndex & myself, MinionsIteratorWithIndex & added_minion)
{
	this->auras.HookAfterMinionAdded(board, myself, added_minion);
}

inline void Minion::HookMinionCheckEnraged(Board & board, MinionsIteratorWithIndex & myself)
{
	if (this->stat.GetHP() < this->stat.GetMaxHP()) {
		this->auras.HookAfterOwnerEnraged(board, myself); // enraged
	}
	else if (this->stat.GetHP() == this->stat.GetMaxHP()) {
		this->auras.HookAfterOwnerUnEnraged(board, myself); // unenraged
	}
	else {
		throw std::runtime_error("hp should not be larger than max-hp");
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

inline void Minion::IncreaseCurrentAndMaxHP(int val)
{
#ifdef DEBUG
	if (val < 0) throw std::runtime_error("should we trigger heal? enrage effect? damaged effect? use TakeDamage() for that.");
#endif
	this->stat.SetMaxHP(this->stat.GetMaxHP() + val);
	this->stat.SetHP(this->stat.GetHP() + val);
}

inline void Minion::DecreaseMaxHP(int val)
{
	this->stat.SetMaxHP(this->stat.GetMaxHP() - val);
	this->stat.SetHP(std::min(this->stat.GetHP(), this->stat.GetMaxHP()));
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

	if (this->enchantments != rhs.enchantments) return false;

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
			
			GameEngine::hash_combine(result, s.stat);
			
			GameEngine::hash_combine(result, s.attacked_times);
			GameEngine::hash_combine(result, s.summoned_this_turn);

			GameEngine::hash_combine(result, s.enchantments);

			return result;
		}
	};
}