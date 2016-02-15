#pragma once

#include <stdexcept>
#include <sstream>

#include "weapon.h"
#include "hero-power.h"
#include "object-base.h"

namespace GameEngine {
namespace BoardObjects{

class HeroManipulator;

class Hero
{
	friend std::hash<Hero>;
	friend class HeroManipulator;

public:
	bool operator==(Hero const& rhs) const;
	bool operator!=(Hero const& rhs) const;

public:
	void Set(int hp, int armor);

public:
	std::string GetDebugString() const;

private:
	int hp;
	int armor;

	Weapon weapon;
	HeroPower hero_power;
};

} // BoardObjects
} // GameEngine

inline bool GameEngine::BoardObjects::Hero::operator==(Hero const & rhs) const
{
	if (this->hp != rhs.hp) return false;
	if (this->armor != rhs.armor) return false;
	if (this->weapon != rhs.weapon) return false;
	if (this->hero_power != rhs.hero_power) return false;

	return true;
}

inline bool GameEngine::BoardObjects::Hero::operator!=(Hero const & rhs) const
{
	return !(*this == rhs);
}

inline void GameEngine::BoardObjects::Hero::Set(int hp, int armor)
{
	this->hp = hp;
	this->armor = armor;
}

inline std::string GameEngine::BoardObjects::Hero::GetDebugString() const
{
	std::ostringstream oss;
	oss << "HP: " << this->hp << " + " << this->armor << std::endl;

	return oss.str();
}

namespace std {
	template <> struct hash<GameEngine::BoardObjects::Hero> {
		typedef GameEngine::BoardObjects::Hero argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			GameEngine::hash_combine(result, s.hp);
			GameEngine::hash_combine(result, s.armor);
			GameEngine::hash_combine(result, s.weapon);
			GameEngine::hash_combine(result, s.hero_power);

			return result;
		}
	};
}
