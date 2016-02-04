#ifndef GAME_ENGINE_CARD_H
#define GAME_ENGINE_CARD_H

#include <functional>
#include <string>
#include <sstream>
#include "common.h"

namespace GameEngine {

class Card
{
	public:
		enum Rarity
		{
			RARITY_FREE,
			RARITY_COMMON,
			RARITY_RARE,
			RARITY_EPIC,
			RARITY_LEGENDARY,
			RARITY_UNKNOWN
		};

	public:
		Card() : id(0) {}

		bool operator==(const Card &rhs) const;
		bool operator!=(const Card &rhs) const;

		void MarkInvalid();
		bool IsValid() const;
		std::string GetDebugString() const;

	public:
		int id;

		Rarity rarity;

		enum Type {
			TYPE_INVALID = 0, // the smallest must be zero
			TYPE_MINION,
			TYPE_SPELL,
			TYPE_WEAPON,
			TYPE_SECRET,
			TYPE_MAX = TYPE_SECRET // must be the largest
		} type;

		enum MinionRace {
			RACE_NORMAL,
			RACE_BEAST,
			RACE_DEMON,
			RACE_DRAGON,
			RACE_MECH,
			RACE_MURLOC,
			RACE_PIRATE,
			RACE_TOTEM
		};

		int cost;

		struct Minion {
			int attack;
			int hp;
			MinionRace race;
			bool taunt;
			bool charge;
			bool shield;
			bool stealth;

			void Clear() {
				this->attack = 0;
				this->hp = 0;
				this->race = RACE_NORMAL;
				this->taunt = false;
				this->charge = false;
				this->shield = false;
				this->stealth = false;
			}

			bool operator==(const Minion &rhs) const {
				if (this->attack != rhs.attack) return false;
				if (this->hp != rhs.hp) return false;
				if (this->race != rhs.race) return false;
				if (this->taunt != rhs.taunt) return false;
				if (this->charge != rhs.charge) return false;
				if (this->shield != rhs.shield) return false;
				if (this->stealth != rhs.stealth) return false;
				return true;
			}

			bool operator!=(const Minion &rhs) const {
				return !(*this == rhs);
			}
		};

		struct Spell {
			// TODO

			void Clear() {
			}
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

inline std::string Card::GetDebugString() const
{
	if (!this->IsValid()) return "INVALID";

	std::ostringstream oss;

	if (this->type == TYPE_MINION)
	{
		oss << "M" << this->cost << this->data.minion.attack << this->data.minion.hp;
	}
	else if (this->type == TYPE_SPELL)
	{
		oss << "S" << this->cost;
	}
	else {
		oss << "???";
	}

	return oss.str();
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

		case TYPE_INVALID:
		case TYPE_SPELL:
		case TYPE_SECRET:
			break;
	}

	return true;
}

inline bool Card::operator!=(const Card &rhs) const
{
	return !(*this == rhs);
}

} // namespace GameEngine

namespace std {
	template <> struct hash<GameEngine::Card::Minion> {
		typedef GameEngine::Card::Minion argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			GameEngine::hash_combine(result, s.attack);
			GameEngine::hash_combine(result, s.hp);
			GameEngine::hash_combine(result, (int)s.race);
			GameEngine::hash_combine(result, s.taunt);
			GameEngine::hash_combine(result, s.charge);
			GameEngine::hash_combine(result, s.shield);
			GameEngine::hash_combine(result, s.stealth);

			return result;
		}
	};

	template <> struct hash<GameEngine::Card::Weapon> {
		typedef GameEngine::Card::Weapon argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			GameEngine::hash_combine(result, s.attack);
			GameEngine::hash_combine(result, s.durability);

			return result;
		}
	};

	template <> struct hash<GameEngine::Card> {
		typedef GameEngine::Card argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			GameEngine::hash_combine(result, s.id);

			GameEngine::hash_combine(result, (int)s.type); // TODO
			GameEngine::hash_combine(result, s.cost);

			switch (s.type) {
				case GameEngine::Card::TYPE_MINION:
					GameEngine::hash_combine(result, s.data.minion);
					break;

				case GameEngine::Card::TYPE_WEAPON:
					GameEngine::hash_combine(result, s.data.weapon);
					break;

				case GameEngine::Card::TYPE_INVALID:
				case GameEngine::Card::TYPE_SPELL:
				case GameEngine::Card::TYPE_SECRET:
					break;
			}

			return result;
		}
	};

}

#endif
