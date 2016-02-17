#pragma once

#include "minion-iterator.h"

namespace GameEngine {
	namespace BoardObjects {

		inline void MinionConstIteratorWithSlotIndex::GoToNext() 
		{
			if (!this->it->pending_removal) SlotIndexHelper::Increase(this->slot_idx);
			this->it++;
		}

		inline bool MinionConstIteratorWithSlotIndex::IsEnd() const
		{
			return this->it == this->minions.end(); 
		}


		inline void MinionIterator::GoToNext() { this->it++; }
		inline bool MinionIterator::IsEnd() const { return this->it == this->minions.end(); }

	} // BoardObjects
} // GameEngine
