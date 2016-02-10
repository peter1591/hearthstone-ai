#ifndef GAME_ENGINE_BOARD_OBJECTS_OBJECT_MANAGER_H
#define GAME_ENGINE_BOARD_OBJECTS_OBJECT_MANAGER_H

#include <vector>
#include <list>
#include <stdexcept>
#include <iostream>

#include "game-engine/slot-index.h"

#include "hero.h"
#include "minions.h"

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

	int GetPlayerMinionsCount() const { return this->player_minions.GetMinionCount(); }
	MinionsConstIteratorWithIndex GetPlayerMinionsIteratorWithIndex() const { return this->player_minions.GetIteratorWithIndex(SLOT_PLAYER_MINION_START); }
	MinionsIteratorWithIndex GetPlayerMinionsIteratorWithIndex() { return this->player_minions.GetIteratorWithIndex(SLOT_PLAYER_MINION_START); }

	int GetOpponentMinionsCount() const { return this->opponent_minions.GetMinionCount(); }
	MinionsConstIteratorWithIndex GetOpponentMinionsIteratorWithIndex() const { return this->opponent_minions.GetIteratorWithIndex(SLOT_OPPONENT_MINION_START); }
	MinionsIteratorWithIndex GetOpponentMinionsIteratorWithIndex() { return this->opponent_minions.GetIteratorWithIndex(SLOT_OPPONENT_MINION_START); }

	MinionsIteratorWithIndex GetMinionIteratorWithIndex(SlotIndex side) {
		if (SlotIndexHelper::IsPlayerSide(side)) return this->GetPlayerMinionsIteratorWithIndex();
		else return this->GetOpponentMinionsIteratorWithIndex();
	}
	MinionsConstIteratorWithIndex GetMinionIteratorWithIndex(SlotIndex side) const {
		if (SlotIndexHelper::IsPlayerSide(side)) return this->GetPlayerMinionsIteratorWithIndex();
		else return this->GetOpponentMinionsIteratorWithIndex();
	}

public: // hooks
	void PlayerTurnStart();
	void PlayerTurnEnd();
	void OpponentTurnStart();
	void OpponentTurnEnd();

	void HookAfterMinionAdded(Board & board, MinionsIteratorWithIndex & minion);

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
}

inline void ObjectManager::PlayerTurnStart()
{
	this->player_minions.TurnStart(true);
	this->opponent_minions.TurnStart(false);
}

inline void ObjectManager::PlayerTurnEnd()
{
	this->player_minions.TurnEnd(true);
	this->opponent_minions.TurnEnd(false);
}

inline void ObjectManager::OpponentTurnStart()
{
	this->opponent_minions.TurnStart(true);
	this->player_minions.TurnStart(false);
}

inline void ObjectManager::OpponentTurnEnd()
{
	this->opponent_minions.TurnEnd(true);
	this->player_minions.TurnEnd(false);
}

inline void ObjectManager::HookAfterMinionAdded(Board & board, MinionsIteratorWithIndex & minion)
{
	this->player_hero.HookAfterMinionAdded(board, minion);
	this->opponent_hero.HookAfterMinionAdded(board, minion);

	for (auto it = this->GetPlayerMinionsIteratorWithIndex(); !it.IsEnd(); it.GoToNext()) {
		it.HookAfterMinionAdded(board, minion);
	}
	for (auto it = this->GetOpponentMinionsIteratorWithIndex(); !it.IsEnd(); it.GoToNext()) {
		it.HookAfterMinionAdded(board, minion);
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