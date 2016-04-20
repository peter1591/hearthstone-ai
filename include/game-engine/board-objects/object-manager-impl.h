#pragma once

#include "game-engine/board.h"
#include "object-manager.h"

namespace GameEngine
{
	inline BoardObject ObjectManager::GetObject(SlotIndex idx)
	{
		if (idx < SLOT_PLAYER_HERO)
			throw std::runtime_error("invalid argument");
		else if (idx == SLOT_PLAYER_HERO)
			return BoardObject(this->board.player.hero);
		else if (idx < SLOT_OPPONENT_HERO)
			return BoardObject(this->GetMinion(idx));
		else if (idx == SLOT_OPPONENT_HERO)
			return BoardObject(this->board.opponent.hero);
		else if (idx < SLOT_MAX)
			return BoardObject(this->GetMinion(idx));
		else
			throw std::runtime_error("invalid argument");
	}

	inline Player & ObjectManager::GetPlayer(SlotIndex idx)
	{
		if (idx < SLOT_PLAYER_HERO)
			throw std::runtime_error("invalid argument");
		else if (idx == SLOT_PLAYER_HERO)
			return this->board.player;
		else if (idx < SLOT_OPPONENT_HERO)
			throw std::runtime_error("invalid argument");
		else if (idx == SLOT_OPPONENT_HERO)
			return this->board.opponent;
		else if (idx < SLOT_MAX)
			throw std::runtime_error("invalid argument");
		else
			throw std::runtime_error("invalid argument");
	}

	inline ObjectManager::ObjectManager(Board & board) :
		board(board)
	{
	}

	inline MinionConstIteratorWithSlotIndex ObjectManager::GetMinionsIteratorWithIndexAtBeginOfSide(SlotIndex side) const
	{
		if (side == SLOT_PLAYER_SIDE) {
			return this->board.player.minions.GetMinionsIteratorWithIndexAtBegin(SLOT_PLAYER_MINION_START);
		}
		else if (side == SLOT_OPPONENT_SIDE) {
			return this->board.opponent.minions.GetMinionsIteratorWithIndexAtBegin(SLOT_OPPONENT_MINION_START);
		}
		else throw std::runtime_error("invalid argument");
	}

	inline MinionIterator ObjectManager::GetMinionIterator(SlotIndex slot_idx)
	{
		if (slot_idx < SLOT_PLAYER_HERO) throw std::runtime_error("invalid argument");
		else if (slot_idx == SLOT_PLAYER_HERO) throw std::runtime_error("invalid argument");
		else if (slot_idx < SLOT_OPPONENT_HERO) {
			return this->board.player.minions.GetIterator(slot_idx - SLOT_PLAYER_MINION_START);
		}
		else if (slot_idx == SLOT_OPPONENT_HERO) throw std::runtime_error("invalid argument");
		else if (slot_idx < SLOT_MAX) {
			return this->board.opponent.minions.GetIterator(slot_idx - SLOT_OPPONENT_MINION_START);
		}
		else throw std::runtime_error("invalid argument");
	}

	inline MinionIterator ObjectManager::GetMinionIteratorAtBeginOfSide(SlotIndex side)
	{
		if (side == SLOT_PLAYER_SIDE) return this->GetMinionIterator(SLOT_PLAYER_MINION_START);
		else if (side == SLOT_OPPONENT_SIDE) return this->GetMinionIterator(SLOT_OPPONENT_MINION_START);
		else throw std::runtime_error("invalid argument");
	}

	inline Minion & ObjectManager::GetMinion(SlotIndex slot_idx)
	{
		if (slot_idx < SLOT_PLAYER_HERO) throw std::runtime_error("invalid argument");
		else if (slot_idx == SLOT_PLAYER_HERO) throw std::runtime_error("invalid argument");
		else if (slot_idx < SLOT_OPPONENT_HERO) {
			return this->board.player.minions.GetMinion(slot_idx - SLOT_PLAYER_MINION_START);
		}
		else if (slot_idx == SLOT_OPPONENT_HERO) throw std::runtime_error("invalid argument");
		else if (slot_idx < SLOT_MAX) {
			return this->board.opponent.minions.GetMinion(slot_idx - SLOT_OPPONENT_MINION_START);
		}
		else throw std::runtime_error("invalid argument");
	}

} // namespace GameEngine