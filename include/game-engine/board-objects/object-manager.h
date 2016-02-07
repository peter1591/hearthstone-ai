#ifndef GAME_ENGINE_BOARD_OBJECTS_OBJECT_MANAGER_H
#define GAME_ENGINE_BOARD_OBJECTS_OBJECT_MANAGER_H

#include <vector>
#include <list>
#include <stdexcept>
#include <iostream>

#include "game-engine/slot-index.h"

#include "hero.h"
#include "minion.h"

namespace GameEngine {
namespace BoardObjects {

class ObjectManager
{
	friend std::hash<ObjectManager>;

public:
	typedef std::list<Minion> Minions;

	class MinionIterator
	{
	public:
		MinionIterator(SlotIndex slot_idx_begin, Minions::iterator it, Minions &minions) 
			: slot_idx(slot_idx_begin), it(it), minions(&minions) { }

		bool IsEnd() const { return this->it == minions->end(); }

		void GoToNext() { this->it++; SlotIndexHelper::Increase(this->slot_idx); }
		void EraseAndGoToNext(Minions & minions) { 
			this->it = minions.erase(this->it);
		}

		Minions::iterator::pointer operator->() { return this->it.operator->(); }

		SlotIndex GetSlotIdx() const { return this->slot_idx; }

	private:
		SlotIndex slot_idx;
		Minions::iterator it;
		Minions * const minions;
	};

	class MinionConstIterator
	{
	public:
		MinionConstIterator(SlotIndex slot_idx_begin, Minions::const_iterator it, Minions const& minions)
			: slot_idx(slot_idx_begin), it(it), minions(&minions) { }

		bool IsEnd() const { return this->it == minions->end(); }

		void GoToNext() { this->it++; SlotIndexHelper::Increase(this->slot_idx); }

		Minions::const_iterator::pointer operator->() { return this->it.operator->(); }

		SlotIndex GetSlotIdx() const { return this->slot_idx; }

	private:
		SlotIndex slot_idx;
		Minions::const_iterator it;
		Minions const* const minions;
	};

public:
	ObjectManager();

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

	bool IsMinionsFull(SlotIndex idx) const;
	bool IsPlayerMinionsFull() const;
	bool IsOpponentMinionsFull() const;

	int GetPlayerMinionsCount() const { return (int)this->player_minions.size(); }
	Minions::iterator PlayerMinionsBegin() { return this->player_minions.begin(); }
	Minions::iterator PlayerMinionsEnd() { return this->player_minions.end(); }
	Minions::const_iterator PlayerMinionsBegin() const { return this->player_minions.cbegin(); }
	Minions::const_iterator PlayerMinionsEnd() const { return this->player_minions.cend(); }
	MinionConstIterator GetPlayerMinionsIterator() const { return MinionConstIterator(SLOT_PLAYER_MINION_START, this->player_minions.begin(), this->player_minions); }
	MinionIterator GetPlayerMinionsIterator() { return MinionIterator(SLOT_PLAYER_MINION_START, this->player_minions.begin(), this->player_minions); }

	int GetOpponentMinionsCount() const { return (int)this->opponent_minions.size(); }
	Minions::iterator OpponentMinionsBegin() { return this->opponent_minions.begin(); }
	Minions::iterator OpponentMinionsEnd() { return this->opponent_minions.end(); }
	Minions::const_iterator OpponentMinionsBegin() const { return this->opponent_minions.cbegin(); }
	Minions::const_iterator OpponentMinionsEnd() const { return this->opponent_minions.cend(); }
	MinionConstIterator GetOpponentMinionsIterator() const { return MinionConstIterator(SLOT_OPPONENT_MINION_START, this->opponent_minions.begin(), this->opponent_minions); }
	MinionIterator GetOpponentMinionsIterator() { return MinionIterator(SLOT_OPPONENT_MINION_START, this->opponent_minions.begin(), this->opponent_minions); }

	MinionIterator GetMinionsIterator(SlotIndex side) { 
		if (SlotIndexHelper::IsPlayerSide(side)) return this->GetPlayerMinionsIterator();
		else return this->GetOpponentMinionsIterator();
	}
	MinionConstIterator GetMinionsIterator(SlotIndex side) const {
		if (SlotIndexHelper::IsPlayerSide(side)) return this->GetPlayerMinionsIterator();
		else return this->GetOpponentMinionsIterator();
	}

	Minion & AddMinion(SlotIndex idx, Minion const& minion);

	void EraseMinion(MinionIterator &it);

public: // hooks
	void PlayerTurnStart();
	void PlayerTurnEnd();
	void OpponentTurnStart();
	void OpponentTurnEnd();

public:
	void DebugPrint() const;

private:
	static constexpr int max_minions = 7;

private:
	static Minions::iterator GetMinionIterator(Minions & minions, int idx);

private:
	Hero player_hero;
	Hero opponent_hero;

	Minions player_minions;
	Minions opponent_minions;
};

inline ObjectManager::ObjectManager()
{
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
			return &(*GetMinionIterator(this->player_minions, idx - SLOT_PLAYER_MINION_START));
		else if (idx == SLOT_OPPONENT_HERO)
			return &this->opponent_hero;
		else if (idx < SLOT_MAX)
			return &(*GetMinionIterator(this->opponent_minions, idx - SLOT_OPPONENT_MINION_START));
		else
			throw std::runtime_error("invalid argument");
	}
}

inline bool ObjectManager::IsMinionsFull(SlotIndex idx) const
{
	if (idx < SLOT_PLAYER_HERO) {
		throw std::runtime_error("invalid argument");
	}
	else if (idx < SLOT_OPPONENT_HERO) {
		return this->IsPlayerMinionsFull(); // player's side
	}
	else if (idx < SLOT_MAX) {
		return this->IsOpponentMinionsFull(); // opponent's side
	}
	else {
		throw std::runtime_error("invalid argument");
	}
}

inline bool ObjectManager::IsPlayerMinionsFull() const
{
	return (this->player_minions.size() >= max_minions);
}

inline bool ObjectManager::IsOpponentMinionsFull() const
{
	return (this->opponent_minions.size() >= max_minions);
}

inline Minion & GameEngine::BoardObjects::ObjectManager::AddMinion(SlotIndex idx, Minion const& minion)
{
	if (idx < SLOT_PLAYER_HERO) {
		throw std::runtime_error("invalid argument");
	}
	else if (idx == SLOT_PLAYER_HERO) {
		throw std::runtime_error("invalid argument");
	}
	else if (idx < SLOT_OPPONENT_HERO) {
		// add to player's side
		if (this->player_minions.size() >= max_minions) throw std::runtime_error("minion full");
		
		int minions_idx = idx - SLOT_PLAYER_MINION_START;
		if (minions_idx > this->player_minions.size()) throw std::runtime_error("invalid argument");

		auto it = GetMinionIterator(this->player_minions, minions_idx);
		this->player_minions.insert(it, minion);
	}
	else if (idx == SLOT_OPPONENT_HERO) {
		throw std::runtime_error("invalid argument");
	}
	else if (idx < SLOT_MAX) {
		// add to opponent's side
		if (this->opponent_minions.size() >= max_minions) throw std::runtime_error("minion full");

		int minions_idx = idx - SLOT_OPPONENT_MINION_START;
		if (minions_idx > this->opponent_minions.size()) throw std::runtime_error("invalid argument");

		auto it = GetMinionIterator(this->opponent_minions, minions_idx);
		this->opponent_minions.insert(it, minion);
	}
	else {
		throw std::runtime_error("invalid argument");
	}
}

inline void ObjectManager::EraseMinion(MinionIterator & it)
{
	if (SlotIndexHelper::IsPlayerSide(it.GetSlotIdx())) it.EraseAndGoToNext(this->player_minions);
	else it.EraseAndGoToNext(this->opponent_minions);
}

inline void ObjectManager::PlayerTurnStart()
{
	for (auto & minion : this->player_minions) minion.TurnStart(true);
	for (auto & minion : this->opponent_minions) minion.TurnStart(false);
}

inline void ObjectManager::PlayerTurnEnd()
{
	for (auto & minion : this->player_minions) minion.TurnEnd(true);
	for (auto & minion : this->opponent_minions) minion.TurnEnd(false);
}

inline void ObjectManager::OpponentTurnStart()
{
	for (auto & minion : this->opponent_minions) minion.TurnStart(true);
	for (auto & minion : this->player_minions) minion.TurnStart(false);
}

inline void ObjectManager::OpponentTurnEnd()
{
	for (auto & minion : this->opponent_minions) minion.TurnEnd(true);
	for (auto & minion : this->player_minions) minion.TurnEnd(false);
}

inline void ObjectManager::DebugPrint() const
{
	std::cout << "Player Hero: " << this->player_hero.GetDebugString() << std::endl;
	std::cout << "Opponent Hero: " << this->opponent_hero.GetDebugString() << std::endl;

	std::cout << "Opponent minions: " << std::endl;
	for (const auto &minion : this->opponent_minions) {
		std::cout << "\t" << minion.GetDebugString() << std::endl;
	}

	std::cout << "Player minions: " << std::endl;
	for (const auto &minion : this->player_minions) {
		std::cout << "\t" << minion.GetDebugString() << std::endl;
	}
}

inline ObjectManager::Minions::iterator ObjectManager::GetMinionIterator(Minions & minions, int idx)
{
	ObjectManager::Minions::iterator it = minions.begin();
	for (; idx > 0; --idx) ++it;
	return it;
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
			for (auto const& minion : s.player_minions) {
				GameEngine::hash_combine(result, minion);
			}

			GameEngine::hash_combine(result, s.opponent_hero);
			for (auto const& minion : s.opponent_minions) {
				GameEngine::hash_combine(result, minion);
			}

			return result;
		}
	};
}
#endif