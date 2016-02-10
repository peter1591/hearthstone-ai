#pragma once

#include <stdexcept>
#include <sstream>

#include "minions-iterator.h"
#include "weapon.h"
#include "hero-power.h"
#include "object-base.h"

namespace GameEngine {
namespace BoardObjects{

class Hero : public ObjectBase
{
	friend std::hash<Hero>;

public:
	bool operator==(Hero const& rhs) const;
	bool operator!=(Hero const& rhs) const;

public:
	void Set(int hp, int armor);

	int GetHP() const { return this->hp; }
	int GetAttack() const;

	void TakeDamage(int);

	void AttackedOnce();

	bool IsForgetful() const;

	void SetFreezeAttacker(bool freeze);
	void SetFreezed(bool freezed);
	bool IsFreezeAttacker() const;
	bool IsFreezed() const;

public: // hooks
	void HookAfterMinionAdded(Board & board, MinionsIteratorWithIndex & minion);

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

inline int GameEngine::BoardObjects::Hero::GetAttack() const
{ 
	throw std::runtime_error("not yet implemented");
}

inline void GameEngine::BoardObjects::Hero::TakeDamage(int damage)
{
	this->hp -= damage;
}

inline void GameEngine::BoardObjects::Hero::AttackedOnce()
{
}

inline bool GameEngine::BoardObjects::Hero::IsForgetful() const
{
	throw std::runtime_error("not yet implemented");
}

inline void GameEngine::BoardObjects::Hero::SetFreezeAttacker(bool)
{
	throw std::runtime_error("not yet implemented");
}

inline void GameEngine::BoardObjects::Hero::SetFreezed(bool)
{
	// TODO
}

inline bool GameEngine::BoardObjects::Hero::IsFreezeAttacker() const
{
	return false;
}

inline bool GameEngine::BoardObjects::Hero::IsFreezed() const
{
	throw std::runtime_error("not yet implemented");
}

inline void GameEngine::BoardObjects::Hero::HookAfterMinionAdded(Board & board, MinionsIteratorWithIndex & minion)
{
	// TODO
	return;
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
