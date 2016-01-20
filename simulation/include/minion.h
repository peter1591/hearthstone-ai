#ifndef MINION_H
#define MINION_H

#include "card.h"

class Minion
{
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

		bool IsValid() const
		{
			return this->card_id != 0;
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

#endif
