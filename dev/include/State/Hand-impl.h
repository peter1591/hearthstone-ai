#pragma once

#include "State/Hand.h"
#include "Manipulators/MinionManipulator.h"

namespace State
{
	void Hand::LocationManipualtor::Insert(EntitiesManager & mgr, CardRef card_ref)
	{
		int pos = mgr.Get(card_ref).GetZonePosition();
		if (pos < 0) throw std::exception("invalid position");
		if (pos > cards_.size()) throw std::exception("invalid position");

		auto it = cards_.insert(cards_.begin() + pos, card_ref);

		++it;
		++pos;

		for (auto it_end = cards_.end(); it != it_end; ++it, ++pos)
		{
			mgr.GetGeneralManipulator(*it).GetZonePositionSetter().Set(pos);
		}
	}

	void Hand::LocationManipualtor::Remove(EntitiesManager & mgr, int pos)
	{
		if (pos < 0) throw std::exception("invalid position");
		if (pos >= cards_.size()) throw std::exception("invalid position");

		auto it = cards_.erase(cards_.begin() + pos);
		for (auto it_end = cards_.end(); it != it_end; ++it, ++pos)
		{
			mgr.GetMinionManipulator(*it).GetZonePositionSetter().Set(pos);
		}
	}
}