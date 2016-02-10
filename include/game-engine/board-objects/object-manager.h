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

namespace GameEngine {
namespace BoardObjects {

class ObjectManager
{
	friend std::hash<ObjectManager>;

public:
	ObjectManager();
	~ObjectManager() {}

	ObjectManager(ObjectManager const& rhs) = delete;
	ObjectManager& operator=(ObjectManager const& rhs) = delete;

	ObjectManager(ObjectManager && rhs) { *this = std::move(rhs); }
	ObjectManager& operator=(ObjectManager && rhs);

	ObjectManager & CloneFrom(ObjectManager const& rhs);

	bool operator==(ObjectManager const& rhs) const;
	bool operator!=(ObjectManager const& rhs) const;

public: // Manipulate heros
	Hero * GetPlayerHero() { return &this->player_hero; }
	Hero const* GetPlayerHero() const { return &this->player_hero; }

	Hero * GetOpponentHero() { return &this->opponent_hero; }
	Hero const* GetOpponentHero() const { return &this->opponent_hero; }

	void SetHero(Hero const& player, Hero const& opponent);

public: // Manipulate minions
	ObjectBase* GetObject(SlotIndex idx);

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
	Hero player_hero;
	Hero opponent_hero;

	Minions player_minions;
	Minions opponent_minions;
};

inline ObjectManager::ObjectManager()
{
}

inline ObjectManager & ObjectManager::operator=(ObjectManager && rhs)
{
	this->player_hero = std::move(rhs.player_hero);
	this->opponent_hero = std::move(rhs.opponent_hero);
	this->player_minions = std::move(rhs.player_minions);
	this->opponent_minions = std::move(rhs.opponent_minions);

	return *this;
}

inline ObjectManager & ObjectManager::CloneFrom(ObjectManager const & rhs)
{
	using GameEngine::BoardObjects::Minion;

	this->player_hero = rhs.player_hero;
	this->opponent_hero = rhs.opponent_hero;

	// copy minions
	this->player_minions.CloneFrom(rhs.player_minions);
	this->opponent_minions.CloneFrom(rhs.opponent_minions);

	return *this;
}

inline bool ObjectManager::operator==(ObjectManager const & rhs) const
{
	if (this->player_hero != rhs.player_hero) return false;
	if (this->opponent_hero != rhs.opponent_hero) return false;

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
	this->player_hero = player;
	this->opponent_hero = opponent;
}

inline ObjectBase * ObjectManager::GetObject(SlotIndex idx)
{
	if (idx < SLOT_PLAYER_HERO)
		throw std::runtime_error("invalid argument");
	else if (idx == SLOT_PLAYER_HERO)
		return &this->player_hero;
	else if (idx < SLOT_OPPONENT_HERO)
		return &this->player_minions.Get(idx - SLOT_PLAYER_MINION_START);
	else if (idx == SLOT_OPPONENT_HERO)
		return &this->opponent_hero;
	else if (idx < SLOT_MAX)
		return &this->opponent_minions.Get(idx - SLOT_OPPONENT_MINION_START);
	else
		throw std::runtime_error("invalid argument");
}

inline MinionManipulator ObjectManager::GetMinionManipulator(GameEngine::Board & board, SlotIndex slot_idx)
{
	int minion_idx;
	Minions * minions = nullptr;

	if (slot_idx < SLOT_PLAYER_HERO) throw std::runtime_error("invalid argument");
	else if (slot_idx == SLOT_PLAYER_HERO) throw std::runtime_error("invalid argument");
	else if (slot_idx < SLOT_OPPONENT_HERO) {
		minions = &this->player_minions;
		minion_idx = slot_idx - SLOT_PLAYER_MINION_START;
		return MinionManipulator(board, *minions, minions->Get(minion_idx));
	}
	else if (slot_idx == SLOT_OPPONENT_HERO) throw std::runtime_error("invalid argument");
	else if (slot_idx < SLOT_MAX) {
		minions = &this->opponent_minions;
		minion_idx = slot_idx - SLOT_OPPONENT_MINION_START;
		return MinionManipulator(board, *minions, minions->Get(minion_idx));
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
	for (auto it = this->GetMinionInserterAtBeginOfSide(board, SLOT_PLAYER_SIDE); !it.IsEnd(); it.GoToNext()) {
		it.ConverToManipulator().TurnStart(true);
	}
	for (auto it = this->GetMinionInserterAtBeginOfSide(board, SLOT_OPPONENT_SIDE); !it.IsEnd(); it.GoToNext()) {
		it.ConverToManipulator().TurnStart(false);
	}
}

inline void ObjectManager::PlayerTurnEnd(GameEngine::Board & board)
{
	for (auto it = this->GetMinionInserterAtBeginOfSide(board, SLOT_PLAYER_SIDE); !it.IsEnd(); it.GoToNext()) {
		it.ConverToManipulator().TurnEnd(true);
	}
	for (auto it = this->GetMinionInserterAtBeginOfSide(board, SLOT_OPPONENT_SIDE); !it.IsEnd(); it.GoToNext()) {
		it.ConverToManipulator().TurnEnd(false);
	}
}

inline void ObjectManager::OpponentTurnStart(GameEngine::Board & board)
{
	for (auto it = this->GetMinionInserterAtBeginOfSide(board, SLOT_OPPONENT_SIDE); !it.IsEnd(); it.GoToNext()) {
		it.ConverToManipulator().TurnStart(true);
	}
	for (auto it = this->GetMinionInserterAtBeginOfSide(board, SLOT_PLAYER_SIDE); !it.IsEnd(); it.GoToNext()) {
		it.ConverToManipulator().TurnStart(false);
	}
}

inline void ObjectManager::OpponentTurnEnd(GameEngine::Board & board)
{
	for (auto it = this->GetMinionInserterAtBeginOfSide(board, SLOT_OPPONENT_SIDE); !it.IsEnd(); it.GoToNext()) {
		it.ConverToManipulator().TurnEnd(true);
	}
	for (auto it = this->GetMinionInserterAtBeginOfSide(board, SLOT_PLAYER_SIDE); !it.IsEnd(); it.GoToNext()) {
		it.ConverToManipulator().TurnEnd(false);
	}
}

inline void ObjectManager::HookAfterMinionAdded(MinionManipulator & added_minion)
{
	this->player_hero.HookAfterMinionAdded(added_minion);
	this->opponent_hero.HookAfterMinionAdded(added_minion);

	for (auto it = this->GetMinionInserterAtBeginOfSide(*added_minion.board, SLOT_PLAYER_SIDE); !it.IsEnd(); it.GoToNext()) {
		it.ConverToManipulator().HookAfterMinionAdded(added_minion);
	}
	for (auto it = this->GetMinionInserterAtBeginOfSide(*added_minion.board, SLOT_OPPONENT_SIDE); !it.IsEnd(); it.GoToNext()) {
		it.ConverToManipulator().HookAfterMinionAdded(added_minion);
	}
}

inline void ObjectManager::DebugPrint() const
{
	std::cout << "Player Hero: " << this->player_hero.GetDebugString() << std::endl;
	std::cout << "Opponent Hero: " << this->opponent_hero.GetDebugString() << std::endl;

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

			GameEngine::hash_combine(result, s.player_hero);
			GameEngine::hash_combine(result, s.player_minions);

			GameEngine::hash_combine(result, s.opponent_hero);
			GameEngine::hash_combine(result, s.opponent_minions);

			return result;
		}
	};
}
#endif