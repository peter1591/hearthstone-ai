#pragma once

#include "game-engine/board.h"
#include "minions.h"

namespace GameEngine {
	namespace BoardObjects {
		
		inline Minion & Minions::InsertBefore(MinionIterator const& it, MinionData && minion)
		{
			auto new_minion = Minion(this->board, *this, std::move(minion));

			auto new_it = this->minions.insert(it.GetIterator(), std::move(new_minion));
#ifdef DEBUG
			this->change_id++;
#endif

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

		inline void Minions::MarkPendingRemoval(Minion & minion)
		{
			if (minion.minion.pending_removal) return;
			minion.minion.pending_removal = true;
			this->pending_removal_count++;
		}

		inline void Minions::EraseAndGoToNext(MinionIterator & it)
		{
			if (it.GetMinion().pending_removal) this->pending_removal_count--;
			it.it = this->minions.erase(it.it);
#ifdef DEBUG
			this->change_id++; // TODO: test only
			it.container_change_id = this->change_id;
#endif
		}
	}
}