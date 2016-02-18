#ifndef GAME_ENGINE_BOARD_OBJECTS_OBJECT_MANAGER_H
#define GAME_ENGINE_BOARD_OBJECTS_OBJECT_MANAGER_H

#include <vector>
#include <list>
#include <stdexcept>
#include <iostream>

#include "game-engine/slot-index.h"

#include "hero.h"
#include "minions.h"
#include "minion-manipulator.h"
#include "hero-manipulator.h"

namespace GameEngine {
namespace BoardObjects {

class BoardObject
{
public:
	explicit BoardObject(HeroManipulator & hero) : hero(hero), minion(*(MinionManipulator*)(nullptr))
	{
		this->ptr = &this->hero;
	}

	explicit BoardObject(MinionManipulator & minion) : minion(minion), hero(*(HeroManipulator*)(nullptr))
	{
		this->ptr = &this->minion;
	}

	bool IsHero() const { return this->ptr == &this->hero; }
	bool IsMinion() const { return this->ptr == &this->minion; }

	HeroManipulator & GetHero()
	{
#ifdef DEBUG
		if (this->IsHero() == false) throw std::runtime_error("type not match.");
#endif
		return this->hero;
	}

	MinionManipulator GetMinion()
	{
#ifdef DEBUG
		if (this->IsMinion() == false) throw std::runtime_error("type not match.");
#endif
		return this->minion;
	}

	ObjectBase * operator->() const { return this->ptr; }

private:
	HeroManipulator & hero;
	MinionManipulator & minion;
	ObjectBase * ptr;
};

class ObjectManager
{
	friend std::hash<ObjectManager>;

public:
	ObjectManager(Board & board);
	~ObjectManager() {}

	ObjectManager(ObjectManager const& rhs) = delete;
	ObjectManager(Board & board, ObjectManager const& rhs);
	ObjectManager& operator=(ObjectManager const& rhs) = delete;

	ObjectManager(Board & board, ObjectManager && rhs);
	ObjectManager& operator=(ObjectManager && rhs);

	bool operator==(ObjectManager const& rhs) const;
	bool operator!=(ObjectManager const& rhs) const;

public: // Get manipulate object
	BoardObject GetObject(SlotIndex idx);

public: // Manipulate heros
	void SetHero(Hero const& player, Hero const& opponent);
	bool IsPlayerHeroAttackable() const { return this->player_hero.GetHero().Attackable(); }
	bool IsOpponentHeroAttackable() const { return this->opponent_hero.GetHero().Attackable(); }

	HeroManipulator & GetHeroBySide(SlotIndex side);

public: // Manipulate minions
	MinionConstIteratorWithSlotIndex GetPlayerMinionsIteratorWithIndex() const { 
		return MinionConstIteratorWithSlotIndex(this->player_minions, this->player_minions.begin(), SLOT_PLAYER_MINION_START);
	}
	MinionConstIteratorWithSlotIndex GetOpponentMinionsIteratorWithIndex() const { 
		return MinionConstIteratorWithSlotIndex(this->opponent_minions, this->opponent_minions.begin(), SLOT_OPPONENT_MINION_START);
	}

	MinionIterator GetMinionIterator(SlotIndex slot_idx);
	MinionIterator GetMinionIteratorAtBeginOfSide(SlotIndex side) {
		if (side == SLOT_PLAYER_SIDE) return this->GetMinionIterator(SLOT_PLAYER_MINION_START);
		else if (side == SLOT_OPPONENT_SIDE) return this->GetMinionIterator(SLOT_OPPONENT_MINION_START);
		else throw std::runtime_error("invalid argument");
	}

	MinionManipulator & GetMinionManipulator(SlotIndex slot_idx);

public: // hooks
	void PlayerTurnStart();
	void PlayerTurnEnd();
	void OpponentTurnStart();
	void OpponentTurnEnd();

	void HookAfterMinionAdded(MinionManipulator & minion);

public:
	void DebugPrint() const;

public:
	HeroManipulator player_hero;
	HeroManipulator opponent_hero;

	Minions player_minions;
	Minions opponent_minions;

private:
	Board & board;
};

inline ObjectManager::ObjectManager(Board & board) :
	board(board), player_hero(board), opponent_hero(board),
	player_minions(board), opponent_minions(board)
{
}

inline ObjectManager::ObjectManager(Board & board, ObjectManager const & rhs) : 
	board(board), player_hero(board), opponent_hero(board),
	player_minions(board), opponent_minions(board)
{
	this->player_hero = rhs.player_hero;
	this->opponent_hero = rhs.opponent_hero;
	this->player_minions = rhs.player_minions;
	this->opponent_minions = rhs.opponent_minions;
}

inline ObjectManager::ObjectManager(Board & board, ObjectManager && rhs) :
	board(board), player_hero(board), opponent_hero(board),
	player_minions(board), opponent_minions(board)
{
	*this = std::move(rhs);
}

inline ObjectManager & ObjectManager::operator=(ObjectManager && rhs)
{
	this->player_hero = std::move(rhs.player_hero);
	this->opponent_hero = std::move(rhs.opponent_hero);
	this->player_minions = std::move(rhs.player_minions);
	this->opponent_minions = std::move(rhs.opponent_minions);

	return *this;
}

inline bool ObjectManager::operator==(ObjectManager const & rhs) const
{
	if (this->player_hero.GetHero() != rhs.player_hero.GetHero()) return false;
	if (this->opponent_hero.GetHero() != rhs.opponent_hero.GetHero()) return false;

	if (this->player_minions != rhs.player_minions) return false;
	if (this->opponent_minions != rhs.opponent_minions) return false;

	return true;
}

inline bool ObjectManager::operator!=(ObjectManager const & rhs) const
{
	return !(*this == rhs);
}

inline void ObjectManager::SetHero(Hero const & player, Hero const & opponent)
{
	this->player_hero.SetHero(player);
	this->opponent_hero.SetHero(opponent);
}

inline HeroManipulator & ObjectManager::GetHeroBySide(SlotIndex side)
{
	if (side == SLOT_PLAYER_SIDE) return this->player_hero;
	else if (side == SLOT_OPPONENT_SIDE) return this->opponent_hero;
	else throw std::runtime_error("invalid argument");
}

inline BoardObject ObjectManager::GetObject(SlotIndex idx)
{
	if (idx < SLOT_PLAYER_HERO)
		throw std::runtime_error("invalid argument");
	else if (idx == SLOT_PLAYER_HERO)
		return BoardObject(this->player_hero);
	else if (idx < SLOT_OPPONENT_HERO)
		return BoardObject(this->GetMinionManipulator(idx));
	else if (idx == SLOT_OPPONENT_HERO)
		return BoardObject(this->opponent_hero);
	else if (idx < SLOT_MAX)
		return BoardObject(this->GetMinionManipulator(idx));
	else
		throw std::runtime_error("invalid argument");
}

inline MinionIterator ObjectManager::GetMinionIterator(SlotIndex slot_idx)
{
	if (slot_idx < SLOT_PLAYER_HERO) throw std::runtime_error("invalid argument");
	else if (slot_idx == SLOT_PLAYER_HERO) throw std::runtime_error("invalid argument");
	else if (slot_idx < SLOT_OPPONENT_HERO) {
		return this->player_minions.GetIterator(slot_idx - SLOT_PLAYER_MINION_START);
	}
	else if (slot_idx == SLOT_OPPONENT_HERO) throw std::runtime_error("invalid argument");
	else if (slot_idx < SLOT_MAX) {
		return this->opponent_minions.GetIterator(slot_idx - SLOT_OPPONENT_MINION_START);
	}
	else throw std::runtime_error("invalid argument");
}

inline MinionManipulator & ObjectManager::GetMinionManipulator(SlotIndex slot_idx)
{
	if (slot_idx < SLOT_PLAYER_HERO) throw std::runtime_error("invalid argument");
	else if (slot_idx == SLOT_PLAYER_HERO) throw std::runtime_error("invalid argument");
	else if (slot_idx < SLOT_OPPONENT_HERO) {
		return this->player_minions.GetManipulator(slot_idx - SLOT_PLAYER_MINION_START);
	}
	else if (slot_idx == SLOT_OPPONENT_HERO) throw std::runtime_error("invalid argument");
	else if (slot_idx < SLOT_MAX) {
		return this->opponent_minions.GetManipulator(slot_idx - SLOT_OPPONENT_MINION_START);
	}
	else throw std::runtime_error("invalid argument");
}

inline void ObjectManager::PlayerTurnStart()
{
	this->player_hero.TurnStart(true);
	this->opponent_hero.TurnStart(false);
	this->player_minions.TurnStart(true);
	this->opponent_minions.TurnStart(false);
}

inline void ObjectManager::PlayerTurnEnd()
{
	this->player_hero.TurnEnd(true);
	this->opponent_hero.TurnEnd(false);
	this->player_minions.TurnEnd(true);
	this->opponent_minions.TurnEnd(false);
}

inline void ObjectManager::OpponentTurnStart()
{
	this->opponent_hero.TurnStart(true);
	this->player_hero.TurnStart(false);
	this->opponent_minions.TurnStart(true);
	this->player_minions.TurnStart(false);
}

inline void ObjectManager::OpponentTurnEnd()
{
	this->opponent_hero.TurnEnd(true);
	this->player_hero.TurnEnd(false);
	this->opponent_minions.TurnEnd(true);
	this->player_minions.TurnEnd(false);
}

inline void ObjectManager::HookAfterMinionAdded(MinionManipulator & added_minion)
{
	this->player_minions.HookAfterMinionAdded(added_minion);
	this->opponent_minions.HookAfterMinionAdded(added_minion);
}

inline void ObjectManager::DebugPrint() const
{
	std::cout << "Player Hero: " << this->player_hero.GetHero().GetDebugString() << std::endl;
	std::cout << "Opponent Hero: " << this->opponent_hero.GetHero().GetDebugString() << std::endl;

	std::cout << "Opponent minions: " << std::endl;
	this->opponent_minions.DebugPrint();

	std::cout << "Player minions: " << std::endl;
	this->player_minions.DebugPrint();
}

} // namespace BoardObjects
} // namespace GameEngine

namespace std {
	template <> struct hash<GameEngine::BoardObjects::ObjectManager> {
		typedef GameEngine::BoardObjects::ObjectManager argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			GameEngine::hash_combine(result, s.player_hero.GetHero());
			GameEngine::hash_combine(result, s.player_minions);

			GameEngine::hash_combine(result, s.opponent_hero.GetHero());
			GameEngine::hash_combine(result, s.opponent_minions);

			return result;
		}
	};
}
#endif