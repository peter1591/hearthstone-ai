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

		inline void MinionsIteratorWithIndex::RemoveAllEffects()
		{
			this->it->Get().RemoveEffects();
		}

		inline void MinionsIteratorWithIndex::EraseAndGoToNext()
		{
			if (this->it->IsPendingRemoval()) this->container.pending_removal_count--;
			this->it = this->container.minions.erase(this->it);
		}

		inline Minion & MinionsIteratorWithIndex::InsertBefore(Minion && minion)
		{
			auto ret = this->container.minions.insert(this->it, std::move(minion));
			SlotIndexHelper::Increase(this->slot_idx);
#ifdef DEBUG
			if (this->slot_idx == SLOT_OPPONENT_SIDE || this->slot_idx == SLOT_MAX) throw std::runtime_error("minion excess range");
#endif
			return ret->Get();
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