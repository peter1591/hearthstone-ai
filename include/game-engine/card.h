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
		Card() : id(0) {}

		bool operator==(const Card &rhs) const;
		bool operator!=(const Card &rhs) const;

		void MarkInvalid();
		bool IsValid() const;
		std::string GetDebugString() const;

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

			GameEngine::hash_combine(result, hash<decltype(s.attack)>()(s.attack));
			GameEngine::hash_combine(result, hash<decltype(s.hp)>()(s.hp));

			return result;
		}
	};

	template <> struct hash<GameEngine::Card::Weapon> {
		typedef GameEngine::Card::Weapon argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			GameEngine::hash_combine(result, hash<decltype(s.attack)>()(s.attack));
			GameEngine::hash_combine(result, hash<decltype(s.durability)>()(s.durability));

			return result;
		}
	};

	template <> struct hash<GameEngine::Card> {
		typedef GameEngine::Card argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			GameEngine::hash_combine(result, hash<decltype(s.id)>()(s.id));

			/*
			GameEngine::hash_combine(result, hash<int>()((int)s.type)); // TODO
			GameEngine::hash_combine(result, hash<decltype(s.cost)>()(s.cost));

			switch (s.type) {
				case GameEngine::Card::TYPE_MINION:
					GameEngine::hash_combine(result, hash<decltype(s.data.minion)>()(s.data.minion));
					break;

				case GameEngine::Card::TYPE_WEAPON:
					GameEngine::hash_combine(result, hash<decltype(s.data.weapon)>()(s.data.weapon));
					break;

				case GameEngine::Card::TYPE_INVALID:
				case GameEngine::Card::TYPE_SPELL:
				case GameEngine::Card::TYPE_SECRET:
					break;
			}
			*/

			return result;
		}
	};

}

#endif
