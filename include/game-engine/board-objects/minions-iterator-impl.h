#pragma once

#include "minions.h"
#include "minions-iterator.h"

namespace GameEngine {
	namespace BoardObjects {

		inline bool MinionsIteratorWithIndex::IsEnd() const
		{
			return this->it == this->container.minions.end();
		}

		inline void MinionsIteratorWithIndex::GoToNext()
		{
			this->it++;
			SlotIndexHelper::Increase(this->slot_idx);
		}

		inline void MinionsIteratorWithIndex::EraseAndGoToNext()
		{
			this->it = this->container.minions.erase(this->it);
		}

		inline Minion & MinionsIteratorWithIndex::InsertBefore(Minion && minion)
		{
			auto ret = this->container.minions.insert(this->it, std::move(minion));
			SlotIndexHelper::Increase(this->slot_idx);
#ifdef DEBUG
			if (this->slot_idx == SLOT_OPPONENT_SIDE || this->slot_idx == SLOT_MAX) throw std::runtime_error("minion excess range");
#endif
			return *ret;
		}

		inline bool MinionsConstIteratorWithIndex::IsEnd() const
		{
			return this->it == this->container.minions.end();
		}

		inline void MinionsConstIteratorWithIndex::GoToNext()
		{
			this->it++; SlotIndexHelper::Increase(this->slot_idx);
		}

	}
}