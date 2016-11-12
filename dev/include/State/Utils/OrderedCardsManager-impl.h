#pragma once

#include "Manipulators/GeneralManipulator.h"

namespace State
{
	namespace Utils
	{
		void OrderedCardsManager::Insert(EntitiesManager & mgr, CardRef card_ref)
		{
			int pos = mgr.Get(card_ref).GetZonePosition();
			if (pos < 0) throw std::exception("invalid position");
			if (pos > container_.size()) throw std::exception("invalid position");

			auto it = container_.insert(container_.begin() + pos, card_ref);

			++it;
			++pos;

			for (auto it_end = container_.end(); it != it_end; ++it, ++pos)
			{
				mgr.GetGeneralManipulator(*it).GetZonePositionSetter().Set(pos);
			}
		}

		void OrderedCardsManager::Remove(EntitiesManager & mgr, size_t pos)
		{
			if (pos >= container_.size()) throw std::exception("invalid position");

			auto it = container_.erase(container_.begin() + pos);
			for (auto it_end = container_.end(); it != it_end; ++it, ++pos)
			{
				mgr.GetMinionManipulator(*it).GetZonePositionSetter().Set(pos);
			}
		}
	}
}