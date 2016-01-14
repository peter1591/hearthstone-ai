#ifndef _CARD_H
#define _CARD_H

class Card
{
	public:
		Card() : id(0) {}

		void MarkInvalid();
		bool IsValid() const;

	public:
		int id;

		enum Type {
			MINION, SPELL, WEAPON, SECRET
		} type;

		struct Minion {
			int cost;
			int attack;
			int hp;
		};

		struct Spell {
			// TODO
		};

		struct Weapon {
			int cost;
			int attack;
			int durability;
		};

		struct Secret {
			// TODO
		};

		union {
			Minion minion;
			Spell spell;
			Weapon weapon;
			Secret secret;
		} data;
};

inline bool Card::IsValid() const
{
	return this->id != 0;
}

inline void Card::MarkInvalid()
{
	this->id = 0;
}

#endif
