#ifndef GAME_ENGINE_MINION_H
#define GAME_ENGINE_MINION_H

#include <list>
#include <memory>
#include <functional>
#include "card.h"

namespace GameEngine {

class Minion
{
	friend std::hash<Minion>;

	public:
		Minion();

		int GetCardId() const { return this->card_id;}
		int GetAttack() const { return this->attack; }
		int GetHP() const { return this->hp; }
		int GetMaxHP() const {return this->max_hp; }

		// Initializer
		void Set(int card_id, int origin_attack, int origin_hp, int origin_max_hp, bool taunt = false);
		void Summon(const Card &card);

		// Modifiers
		void AddAttackThisTurn(int attack);
		void AttackedOnce();
		void TakeDamage(int damage);
		void SetTaunt(bool taunt) { this->taunt = taunt; }

		// Getters
		bool Attackable() const;
		bool IsTaunt() const { return this->taunt; }

		// Hooks
		void TurnStart();
		void TurnEnd();

		bool IsValid() const { return this->card_id != 0; }

		bool operator==(const Minion &rhs) const;
		bool operator!=(const Minion &rhs) const;

	private:
		int card_id;

		// used when silenced
		int origin_attack;
		int origin_hp;
		int origin_max_hp;

		// current stat
		int attack;
		int hp;
		int max_hp;
		bool taunt;

		// enchantments
		int attack_bias_when_turn_ends;

		int attacked_times;
		bool summoned_this_turn;
};

inline Minion::Minion() : card_id(0)
{

}

inline void Minion::Set(int card_id, int origin_attack, int origin_hp, int origin_max_hp, bool taunt)
{
	this->card_id = card_id;
	this->origin_attack = origin_attack;
	this->origin_hp = origin_hp;
	this->origin_max_hp = origin_max_hp;

	this->attack = this->origin_attack;
	this->hp = this->origin_hp;
	this->max_hp = this->origin_max_hp;
	this->taunt = taunt;

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

inline void Minion::TurnStart()
{
	this->summoned_this_turn = false;
	this->attacked_times = 0;
}

inline void Minion::TurnEnd()
{
	this->attack += this->attack_bias_when_turn_ends;
	this->attack_bias_when_turn_ends = 0;
}

inline bool Minion::Attackable() const
{
	if (this->summoned_this_turn) return false;
	if (attacked_times > 0) return false;
	if (this->GetAttack() <= 0) return false;
	return true;
}

inline void Minion::AttackedOnce()
{
	this->attacked_times++;
}

inline void Minion::TakeDamage(int damage)
{
	this->hp -= damage;
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

	if (this->attack_bias_when_turn_ends != rhs.attack_bias_when_turn_ends) return false;
	if (this->attacked_times != rhs.attacked_times) return false;
	if (this->summoned_this_turn != rhs.summoned_this_turn) return false;

	return true;
}

inline bool Minion::operator!=(Minion const& rhs) const
{
	return !(*this == rhs);
}

} // namespace GameEngine

namespace std {

	template <> struct hash<GameEngine::Minion> {
		typedef GameEngine::Minion argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			GameEngine::hash_combine(result, s.card_id);
			GameEngine::hash_combine(result, s.origin_attack);
			GameEngine::hash_combine(result, s.origin_hp);
			GameEngine::hash_combine(result, s.origin_max_hp);
			GameEngine::hash_combine(result, s.taunt);
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

#endif
