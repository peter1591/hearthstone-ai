#pragma once

#include <stdexcept>
#include <sstream>

#include "weapon.h"
#include "hero-power.h"
#include "object-base.h"
#include "enchantments.h"

namespace GameEngine {
namespace BoardObjects{

class HeroManipulator;

class Hero
{
	friend std::hash<Hero>;
	friend class HeroManipulator;

public:
	Hero() : attacked_times(0), freezed(false) {}

	bool operator==(Hero const& rhs) const;
	bool operator!=(Hero const& rhs) const;

public:
	// Getters
	bool Attackable() const;
	int GetAttack() const;

public:
	std::string GetDebugString() const;

public:
	int hp;
	int armor;

	Weapon weapon;
	HeroPower hero_power;

	int attacked_times;
	bool freezed;
};

} // BoardObjects
} // GameEngine

inline bool GameEngine::BoardObjects::Hero::operator==(Hero const & rhs) const
{
	if (this->hp != rhs.hp) return false;
	if (this->armor != rhs.armor) return false;
	if (this->weapon != rhs.weapon) return false;
	if (this->hero_power != rhs.hero_power) return false;
	if (this->attacked_times != rhs.attacked_times) return false;
	if (this->freezed != rhs.freezed) return false;

	return true;
}

inline bool GameEngine::BoardObjects::Hero::operator!=(Hero const & rhs) const
{
	return !(*this == rhs);
}

inline bool GameEngine::BoardObjects::Hero::Attackable() const
{
	if (this->freezed) return false;

	int max_attacked_times = 1;
	if (this->weapon.windfury) max_attacked_times = 2;

	if (this->attacked_times >= max_attacked_times) return false;

	if (this->GetAttack() <= 0) return false;

	return true;
}

inline int GameEngine::BoardObjects::Hero::GetAttack() const
{
	int attack = 0;
	if (this->weapon.IsVaild()) {
		attack += this->weapon.attack;
	}
	// TODO: hero can have its attack value in addition to the weapon
	return attack;
}

inline std::string GameEngine::BoardObjects::Hero::GetDebugString() const
{
	std::ostringstream oss;
	oss << "HP: " << this->hp << " + " << this->armor << std::endl;
	if (this->weapon.IsVaild()) {
		oss << "Weapon: [" << this->weapon.card_id << "] " << this->weapon.attack << " " << this->weapon.durability << std::endl;
	}

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
			GameEngine::hash_combine(result, s.attacked_times);
			GameEngine::hash_combine(result, s.freezed);

			return result;
		}
	};
}
