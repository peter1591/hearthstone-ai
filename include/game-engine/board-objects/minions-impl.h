#pragma once

#include "game-engine/board.h"
#include "minions.h"

namespace GameEngine {
	namespace BoardObjects {
		
		inline MinionManipulator & Minions::InsertBefore(MinionIterator const& it, Minion && minion)
		{
			auto new_minion = MinionManipulator(this->board, *this, std::move(minion));

			auto new_it = this->minions.insert(it.GetIterator(), std::move(new_minion));

			auto & summoned_minion = *new_it;

			this->board.object_manager.HookAfterMinionAdded(summoned_minion);

			return summoned_minion;
		}

		inline void Minions::MarkPendingRemoval(MinionIterator const & it)
		{
			if (it.GetMinion().pending_removal) return;
			it.it->minion.pending_removal = true;
			this->pending_removal_count++;
		}

		inline void Minions::MarkPendingRemoval(MinionManipulator & minion)
		{
			if (minion.minion.pending_removal) return;
			minion.minion.pending_removal = true;
			this->pending_removal_count++;
		}

		inline void Minions::EraseAndGoToNext(MinionIterator & it)
		{
			if (it.GetMinion().pending_removal) this->pending_removal_count--;
			it.it = this->minions.erase(it.it);
		}
	}
}