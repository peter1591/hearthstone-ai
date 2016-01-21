#ifndef CARD_H
#define CARD_H

namespace GameEngine {

class Card
{
	public:
		Card() : id(0) {}

		void MarkInvalid();
		bool IsValid() const;

	public:
		int id;

		enum Type {
			TYPE_INVALID = 0, // the smallest must be zero
			TYPE_MINION,
			TYPE_SPELL,
			TYPE_WEAPON,
			TYPE_SECRET,
			TYPE_MAX = TYPE_SECRET // must be the largest
		} type;

		struct Minion {
			int attack;
			int hp;
		};

		struct Spell {
			// TODO
		};

		struct Weapon {
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

		int cost;
};

inline bool Card::IsValid() const
{
	return this->id != 0;
}

inline void Card::MarkInvalid()
{
	this->id = 0;
}

} // namespace GameEngine

#endif
