#ifndef GAME_ENGINE_CARD_H
#define GAME_ENGINE_CARD_H

namespace GameEngine {

class Card
{
	public:
		Card() : id(0) {}

		void MarkInvalid();
		bool IsValid() const;

		bool operator==(const Card &rhs) const;
		bool operator!=(const Card &rhs) const {
			return !(*this == rhs);
		}

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

		int cost;

		struct Minion {
			int attack;
			int hp;

			bool operator==(const Minion &rhs) const {
				if (this->attack != rhs.attack) return false;
				if (this->hp != rhs.hp) return false;
				return true;
			}

			bool operator!=(const Minion &rhs) const {
				return !(*this == rhs);
			}
		};

		struct Spell {
			// TODO
		};

		struct Weapon {
			int attack;
			int durability;

			bool operator==(const Weapon &rhs) const {
				if (this->attack != rhs.attack) return false;
				if (this->durability != rhs.durability) return false;
				return true;
			}

			bool operator!=(const Weapon &rhs) const {
				return !(*this == rhs);
			}
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

inline bool Card::operator==(const Card &rhs) const
{
	if (this->id != rhs.id) return false;
	if (this->type != rhs.type) return false;
	if (this->cost != rhs.cost) return false;

	switch (this->type) {
		case TYPE_MINION:
			if (this->data.minion != rhs.data.minion) return false;
			break;

		case TYPE_WEAPON:
			if (this->data.weapon != rhs.data.weapon) return false;
			break;
	}

	return true;
}

} // namespace GameEngine

#endif
