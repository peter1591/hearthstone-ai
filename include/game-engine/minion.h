#ifndef GAME_ENGINE_MINION_H
#define GAME_ENGINE_MINION_H

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

		void Set(int card_id, int attack, int hp, int max_hp)
		{
			this->card_id = card_id;
			this->attack = attack;
			this->hp = hp;
			this->max_hp = max_hp;
		}

		void Summon(const Card &card) {
			this->card_id = card.id;
			this->max_hp = card.data.minion.hp;
			this->hp = this->max_hp;
			this->attack = card.data.minion.attack;

			this->attacked_times = 0;
			this->summoned_this_turn = true;
		}

		void TurnStart() {
			this->summoned_this_turn = false;
			this->attacked_times = 0;
		}

		bool Attackable() const
		{
			if (this->summoned_this_turn) return false;
			if (attacked_times > 0) return false;
			return true;
		}

		void AttackedOnce()
		{
			this->attacked_times++;
		}

		void TakeDamage(int damage)
		{
			this->hp -= damage;
		}

		bool IsValid() const
		{
			return this->card_id != 0;
		}

		bool operator==(const Minion &rhs) const
		{
			if (this->card_id != rhs.card_id) return false;
			if (this->attack != rhs.attack) return false;
			if (this->hp != rhs.hp) return false;
			if (this->max_hp != rhs.max_hp) return false;
			if (this->attacked_times != rhs.attacked_times) return false;
			if (this->summoned_this_turn != rhs.summoned_this_turn) return false;

			return true;
		}

		bool operator!=(const Minion &rhs) const
		{
			return !(*this == rhs);
		}

	private:
		int card_id;
		int attack;
		int hp;
		int max_hp;

		int attacked_times;
		bool summoned_this_turn;
};

inline Minion::Minion() : card_id(0)
{

}

} // namespace GameEngine

namespace std {

	template <> struct hash<GameEngine::Minion> {
		typedef GameEngine::Minion argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			GameEngine::hash_combine(result, s.card_id);
			GameEngine::hash_combine(result, s.attack);
			GameEngine::hash_combine(result, s.hp);
			GameEngine::hash_combine(result, s.max_hp);
			GameEngine::hash_combine(result, s.attacked_times);
			GameEngine::hash_combine(result, s.summoned_this_turn);

			return result;
		}
	};

}

#endif
