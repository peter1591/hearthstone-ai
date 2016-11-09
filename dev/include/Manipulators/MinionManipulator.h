#pragma once

#include "Entity/Card.h"
#include "Manipulators/Helpers/BasicHelper.h"

namespace Manipulators
{
	class MinionManipulator
	{
	public:
		MinionManipulator(Card &card) : card_(card) {}

		void ChangeZone(CardZone new_zone)
		{
			card_.SetZone(new_zone);
		}

		void SetCost(int new_cost)
		{
			Helpers::BasicHelper::SetCost(card_, new_cost);
		}

	private:
		Card & card_;
	};
}