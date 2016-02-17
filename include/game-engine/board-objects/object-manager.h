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
	explicit BoardObject(HeroManipulator & hero) : hero(&hero),
		minion(*(Board*)(nullptr), *(Minions*)(nullptr), *(Minion*)(nullptr))
	{
		this->ptr = this->hero;
	}

	explicit BoardObject(MinionManipulator && minion) : minion(minion), hero(nullptr)
	{
		this->ptr = &this->minion;
	}

	bool IsHero() const { return this->ptr == this->hero; }
	bool IsMinion() const { return this->ptr == &this->minion; }

	HeroManipulator & GetHero()
	{
#ifdef DEBUG
		if (this->IsHero() == false) throw std::runtime_error("type not match.");
#endif
		return *this->hero;
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
	HeroManipulator * hero;
	MinionManipulator minion; // a pointer to the actual memory location
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
	BoardObject GetObject(GameEngine::Board & board, SlotIndex idx);
	BoardObject GetPlayerHero();
	BoardObject GetOpponentHero();

public: // Manipulate heros
	void SetHero(Hero const& player, Hero const& opponent);
	bool IsPlayerHeroAttackable() const { return this->player_hero.GetHero().Attackable(); }
	bool IsOpponentHeroAttackable() const { return this->opponent_hero.GetHero().Attackable(); }

	HeroManipulator & GetHeroBySide(GameEngine::Board & board, SlotIndex side);

public: // Manipulate minions
	bool IsPlayerMinionsFull() const { return this->player_minions.IsFull(); }
	bool IsOpponentMinionsFull() const { return this->opponent_minions.IsFull(); }
	bool IsMinionsFull(SlotIndex side) const {
		if (SlotIndexHelper::IsPlayerSide(side)) return this->IsPlayerMinionsFull();
		else return this->IsOpponentMinionsFull();
	}

	bool IsPlayerSide(Minions * minions) const { return &this->player_minions == minions; }
	bool IsOpponentSide(Minions * minions) const { return &this->opponent_minions == minions; }

	int GetPlayerMinionsCount() const { return this->player_minions.GetMinionCount(); }
	MinionConstIteratorWithSlotIndex GetPlayerMinionsIteratorWithIndex() const { return this->player_minions.GetIteratorWithSlotIndex(SLOT_PLAYER_MINION_START); }

	int GetOpponentMinionsCount() const { return this->opponent_minions.GetMinionCount(); }
	MinionConstIteratorWithSlotIndex GetOpponentMinionsIteratorWithIndex() const { return this->opponent_minions.GetIteratorWithSlotIndex(SLOT_OPPONENT_MINION_START); }

	MinionManipulator GetMinionManipulator(GameEngine::Board & board, SlotIndex idx);
	MinionInserter GetMinionInserter(GameEngine::Board & board, SlotIndex idx);

	MinionInserter GetMinionInserterAtBeginOfSide(GameEngine::Board & board, SlotIndex side) {
		if (SlotIndexHelper::IsPlayerSide(side)) return this->GetMinionInserter(board, SLOT_PLAYER_MINION_START);
		else return this->GetMinionInserter(board, SLOT_OPPONENT_MINION_START);
	}

public: // hooks
	void PlayerTurnStart(GameEngine::Board & board);
	void PlayerTurnEnd(GameEngine::Board & board);
	void OpponentTurnStart(GameEngine::Board & board);
	void OpponentTurnEnd(GameEngine::Board & board);

	void HookAfterMinionAdded(MinionManipulator & minion);

public:
	void DebugPrint() const;

private:
	HeroManipulator player_hero;
	HeroManipulator opponent_hero;

	Minions player_minions;
	Minions opponent_minions;
};

inline ObjectManager::ObjectManager(Board & board) :
	player_hero(board), opponent_hero(board)
{
}

inline ObjectManager::ObjectManager(Board & board, ObjectManager const & rhs)
	: player_hero(board), opponent_hero(board)
{
	this->player_hero = rhs.player_hero;
	this->opponent_hero = rhs.opponent_hero;

	// copy minions
	this->player_minions.CloneFrom(rhs.player_minions);
	this->opponent_minions.CloneFrom(rhs.opponent_minions);
}

inline ObjectManager::ObjectManager(Board & board, ObjectManager && rhs) :
	player_hero(board), opponent_hero(board)
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

inline HeroManipulator & ObjectManager::GetHeroBySide(GameEngine::Board & board, SlotIndex side)
{
	if (side == SLOT_PLAYER_SIDE) return this->player_hero;
	else if (side == SLOT_OPPONENT_SIDE) return this->opponent_hero;
	else throw std::runtime_error("invalid argument");
}

inline BoardObject ObjectManager::GetObject(GameEngine::Board & board, SlotIndex idx)
{
	if (idx < SLOT_PLAYER_HERO)
		throw std::runtime_error("invalid argument");
	else if (idx == SLOT_PLAYER_HERO)
		return this->GetPlayerHero();
	else if (idx < SLOT_OPPONENT_HERO)
		return BoardObject(this->GetMinionManipulator(board, idx));
	else if (idx == SLOT_OPPONENT_HERO)
		return this->GetOpponentHero();
	else if (idx < SLOT_MAX)
		return BoardObject(this->GetMinionManipulator(board, idx));
	else
		throw std::runtime_error("invalid argument");
}

inline BoardObject ObjectManager::GetPlayerHero()
{
	return BoardObject(this->player_hero);
}

inline BoardObject ObjectManager::GetOpponentHero()
{
	return BoardObject(this->opponent_hero);
}

inline MinionManipulator ObjectManager::GetMinionManipulator(GameEngine::Board & board, SlotIndex slot_idx)
{
	if (slot_idx < SLOT_PLAYER_HERO) throw std::runtime_error("invalid argument");
	else if (slot_idx == SLOT_PLAYER_HERO) throw std::runtime_error("invalid argument");
	else if (slot_idx < SLOT_OPPONENT_HERO) {
		return this->player_minions.GetManipulator(board, slot_idx - SLOT_PLAYER_MINION_START);
	}
	else if (slot_idx == SLOT_OPPONENT_HERO) throw std::runtime_error("invalid argument");
	else if (slot_idx < SLOT_MAX) {
		return this->opponent_minions.GetManipulator(board, slot_idx - SLOT_OPPONENT_MINION_START);
	}
	else throw std::runtime_error("invalid argument");
}

inline MinionInserter ObjectManager::GetMinionInserter(GameEngine::Board & board, SlotIndex slot_idx)
{
	if (slot_idx < SLOT_PLAYER_HERO) throw std::runtime_error("invalid argument");
	else if (slot_idx == SLOT_PLAYER_HERO) throw std::runtime_error("invalid argument");
	else if (slot_idx < SLOT_OPPONENT_HERO) {
		return this->player_minions.GetInserter(board, slot_idx - SLOT_PLAYER_MINION_START);
	}
	else if (slot_idx == SLOT_OPPONENT_HERO) throw std::runtime_error("invalid argument");
	else if (slot_idx < SLOT_MAX) {
		return this->opponent_minions.GetInserter(board, slot_idx - SLOT_OPPONENT_MINION_START);
	}
	else throw std::runtime_error("invalid argument");
}

inline void ObjectManager::PlayerTurnStart(GameEngine::Board & board)
{
	this->GetPlayerHero().GetHero().TurnStart(true);
	this->GetOpponentHero().GetHero().TurnStart(false);

	for (auto it = this->GetMinionInserterAtBeginOfSide(board, SLOT_PLAYER_SIDE); !it.IsEnd(); it.GoToNext()) {
		it.ConverToManipulator().TurnStart(true);
	}
	for (auto it = this->GetMinionInserterAtBeginOfSide(board, SLOT_OPPONENT_SIDE); !it.IsEnd(); it.GoToNext()) {
		it.ConverToManipulator().TurnStart(false);
	}
}

inline void ObjectManager::PlayerTurnEnd(GameEngine::Board & board)
{
	this->GetPlayerHero().GetHero().TurnEnd(true);
	this->GetOpponentHero().GetHero().TurnEnd(false);

	for (auto it = this->GetMinionInserterAtBeginOfSide(board, SLOT_PLAYER_SIDE); !it.IsEnd(); it.GoToNext()) {
		it.ConverToManipulator().TurnEnd(true);
	}
	for (auto it = this->GetMinionInserterAtBeginOfSide(board, SLOT_OPPONENT_SIDE); !it.IsEnd(); it.GoToNext()) {
		it.ConverToManipulator().TurnEnd(false);
	}
}

inline void ObjectManager::OpponentTurnStart(GameEngine::Board & board)
{
	this->GetOpponentHero().GetHero().TurnStart(true);
	this->GetPlayerHero().GetHero().TurnStart(false);

	for (auto it = this->GetMinionInserterAtBeginOfSide(board, SLOT_OPPONENT_SIDE); !it.IsEnd(); it.GoToNext()) {
		it.ConverToManipulator().TurnStart(true);
	}
	for (auto it = this->GetMinionInserterAtBeginOfSide(board, SLOT_PLAYER_SIDE); !it.IsEnd(); it.GoToNext()) {
		it.ConverToManipulator().TurnStart(false);
	}
}

inline void ObjectManager::OpponentTurnEnd(GameEngine::Board & board)
{
	this->GetOpponentHero().GetHero().TurnEnd(true);
	this->GetPlayerHero().GetHero().TurnEnd(false);

	for (auto it = this->GetMinionInserterAtBeginOfSide(board, SLOT_OPPONENT_SIDE); !it.IsEnd(); it.GoToNext()) {
		it.ConverToManipulator().TurnEnd(true);
	}
	for (auto it = this->GetMinionInserterAtBeginOfSide(board, SLOT_PLAYER_SIDE); !it.IsEnd(); it.GoToNext()) {
		it.ConverToManipulator().TurnEnd(false);
	}
}

inline void ObjectManager::HookAfterMinionAdded(MinionManipulator & added_minion)
{
	for (auto it = this->GetMinionInserterAtBeginOfSide(added_minion.GetBoard(), SLOT_PLAYER_SIDE); !it.IsEnd(); it.GoToNext()) {
		it.ConverToManipulator().HookAfterMinionAdded(added_minion);
	}
	for (auto it = this->GetMinionInserterAtBeginOfSide(added_minion.GetBoard(), SLOT_OPPONENT_SIDE); !it.IsEnd(); it.GoToNext()) {
		it.ConverToManipulator().HookAfterMinionAdded(added_minion);
	}
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