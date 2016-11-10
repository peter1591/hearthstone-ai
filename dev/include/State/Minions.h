#pragma once

#include <vector>
#include "EntitiesManager/EntitiesManager.h"
#include "EntitiesManager/CardRef.h"

namespace State
{
	class Minions
	{
	public:
		Minions()
		{
			minions_.reserve(7);
		}

		void Insert(EntitiesManager & mgr, CardRef card_ref)
		{
			int pos = mgr.Get(card_ref).GetZonePosition();
			if (pos < 0) throw std::exception("invalid position");
			if (pos > minions_.size()) throw std::exception("invalid position");

			auto it = minions_.insert(minions_.begin() + pos, card_ref);

			++it;
			++pos;

			for (auto it_end = minions_.end(); it != it_end; ++it, ++pos)
			{
				mgr.GetMinionManipulator(*it).SetZonePosition(pos);
			}
		}

	private:
		std::vector<CardRef> minions_;
	};
}