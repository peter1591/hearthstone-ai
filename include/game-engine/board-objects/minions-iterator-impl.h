#pragma once

#include "minions.h"
#include "minions-iterator.h"

namespace GameEngine {
	namespace BoardObjects {
		inline void MinionConstIteratorWithSlotIndex::GoToNext()
		{
			if (!this->it->pending_removal) SlotIndexHelper::Increase(this->slot_idx);
			this->it++;
		}

		inline bool MinionConstIteratorWithSlotIndex::IsEnd() const
		{
			return this->it == this->container.end();
		}
	}
}