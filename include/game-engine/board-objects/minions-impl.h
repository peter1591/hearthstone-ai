#pragma once

#include "game-engine/board.h"
#include "minions.h"

namespace GameEngine {
	namespace BoardObjects {
		
		inline MinionManipulator Minions::InsertBefore(MinionIterator const& it, Minion && minion)
		{
			auto new_it = this->minions.insert(it.GetIterator(), std::move(minion));

			auto summoned_minion = GameEngine::BoardObjects::MinionManipulator(this->board, *this, *new_it);

			this->board.object_manager.HookAfterMinionAdded(summoned_minion);

			return summoned_minion;
		}
	}
}