#pragma once

#include "Entity/Card.h"
#include "Manipulators/Helpers/BasicHelper.h"

namespace Manipulators
{
	class MinionManipulator
	{
	public:
		MinionManipulator(Entity::Card &card) : card_(card) {}

		void ChangeZone(Entity::CardZone new_zone)
		{
			card_.SetZone(new_zone);
		}

		void SetCost(int new_cost)
		{
			Helpers::BasicHelper::SetCost(card_, new_cost);
		}

	private:
		Entity::Card & card_;
	};
}