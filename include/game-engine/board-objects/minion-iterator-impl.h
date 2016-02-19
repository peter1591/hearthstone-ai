#pragma once

#include "minion-iterator.h"

namespace GameEngine {
	namespace BoardObjects {

		inline void MinionConstIteratorWithSlotIndex::GoToNext() 
		{
			if (!this->it->GetMinion().pending_removal) SlotIndexHelper::Increase(this->slot_idx);
			this->it++;
		}

		inline bool MinionConstIteratorWithSlotIndex::IsEnd() const
		{
			return this->it == this->minions.end(); 
		}


		inline MinionIterator::MinionIterator(Board & board, Minions & minions, container_type::iterator it)
			: board(board), minions(minions), it(it)
		{
#ifdef DEBUG
			if (it != minions.end()) {
				if (&it->GetBoard() != &board) throw std::runtime_error("consistency check failed");
				if (&it->GetMinions() != &minions) throw std::runtime_error("consistency check failed");
			}
#endif
		}

		inline bool MinionIterator::IsPlayerSide() const
		{
			return &this->minions == &this->board.object_manager.player_minions;
		}

		inline bool MinionIterator::IsOpponentSide() const
		{
			return &this->minions == &this->board.object_manager.opponent_minions;
		}

		inline void MinionIterator::GoToNext() { this->it++; }
		inline bool MinionIterator::IsEnd() const { return this->it == this->minions.end(); }

	} // BoardObjects
} // GameEngine
