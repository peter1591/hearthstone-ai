#pragma once

#include "minions.h"
#include "minions-iterator.h"

namespace GameEngine {
	namespace BoardObjects {
		inline void MinionConstIteratorWithSlotIndex::GoToNext()
		{
			if (!this->IsPendingRemoval()) SlotIndexHelper::Increase(this->slot_idx);
			this->it++;
		}

		inline bool MinionConstIteratorWithSlotIndex::IsEnd() const
		{
			return this->it == this->container.minions.end();
		}

		inline bool MinionConstIteratorWithSlotIndex::IsPendingRemoval() const
		{
			return this->it->pending_removal;
		}
	}
}