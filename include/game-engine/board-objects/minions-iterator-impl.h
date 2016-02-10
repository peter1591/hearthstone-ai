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
			if (!this->IsPendingRemoval()) SlotIndexHelper::Increase(this->slot_idx);
			this->it++;
		}

		inline bool MinionsIteratorWithIndex::IsPendingRemoval() const
		{
			return this->it->IsPendingRemoval();
		}

		inline void MinionsIteratorWithIndex::MarkPendingRemoval()
		{
			if (this->it->IsPendingRemoval()) return;

			this->it->SetPendingRemoval();
			this->container.pending_removal_count++;
		}

		inline void MinionsIteratorWithIndex::EraseAndGoToNext()
		{
			if (this->it->IsPendingRemoval()) this->container.pending_removal_count--;
			this->it = this->container.minions.erase(this->it);
		}

		inline MinionsIteratorWithIndex MinionsIteratorWithIndex::InsertBefore(Minion && minion)
		{
			auto new_it = this->container.minions.insert(this->it, std::move(minion));
			auto new_slot_idx = this->slot_idx;

			if (this->IsEnd() || !this->IsPendingRemoval()) SlotIndexHelper::Increase(this->slot_idx);

#ifdef DEBUG
			if (!this->IsEnd()) {
				if (this->slot_idx == SLOT_OPPONENT_SIDE || this->slot_idx == SLOT_MAX) throw std::runtime_error("minion excess range");
			}
#endif

			return MinionsIteratorWithIndex(new_slot_idx, new_it, this->container);
		}

		inline bool MinionsConstIteratorWithIndex::IsEnd() const
		{
			return this->it == this->container.minions.end();
		}

		inline void MinionsConstIteratorWithIndex::GoToNext()
		{
			this->it++; SlotIndexHelper::Increase(this->slot_idx);
		}

		inline bool MinionsConstIteratorWithIndex::IsPendingRemoval() const
		{
			return this->it->IsPendingRemoval();
		}

	}
}