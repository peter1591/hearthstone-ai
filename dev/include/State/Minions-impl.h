#pragma once

#include "State/Minions.h"
#include "Manipulators/MinionManipulator.h"

namespace State
{
	void Minions::LocationManipualtor::Insert(EntitiesManager & mgr, CardRef card_ref)
	{
		int pos = mgr.Get(card_ref).GetZonePosition();
		if (pos < 0) throw std::exception("invalid position");
		if (pos > minions_.size()) throw std::exception("invalid position");

		auto it = minions_.insert(minions_.begin() + pos, card_ref);

		++it;
		++pos;

		for (auto it_end = minions_.end(); it != it_end; ++it, ++pos)
		{
			mgr.GetMinionManipulator(*it).GetZonePositionSetter().SetZonePosition(pos);
		}
	}

	void Minions::LocationManipualtor::Remove(EntitiesManager & mgr, int pos)
	{
		if (pos < 0) throw std::exception("invalid position");
		if (pos >= minions_.size()) throw std::exception("invalid position");

		auto it = minions_.erase(minions_.begin() + pos);
		for (auto it_end = minions_.end(); it != it_end; ++it, ++pos)
		{
			mgr.GetMinionManipulator(*it).GetZonePositionSetter().SetZonePosition(pos);
		}
	}
}