#pragma once

#include "State/Manipulators/StateManipulator.h"
#include "State/State.h"

namespace state
{
	namespace board
	{
		void OrderedCardsManager::Insert(State & state, CardRef card_ref)
		{
			int pos = state.mgr.Get(card_ref).GetZonePosition();
			if (pos < 0) throw std::exception("invalid position");
			if (pos > container_.size()) throw std::exception("invalid position");

			auto it = container_.insert(container_.begin() + pos, card_ref);

			++it;
			++pos;

			for (auto it_end = container_.end(); it != it_end; ++it, ++pos)
			{
				FlowControl::Manipulators::StateManipulator(state).Card(*it).ZonePosition().Set(pos);
			}
		}

		void OrderedCardsManager::Remove(State & state, size_t pos)
		{
			if (pos >= container_.size()) throw std::exception("invalid position");

			auto it = container_.erase(container_.begin() + pos);
			for (auto it_end = container_.end(); it != it_end; ++it, ++pos)
			{
				FlowControl::Manipulators::StateManipulator(state).Card(*it).ZonePosition().Set(pos);
			}
		}
	}
}