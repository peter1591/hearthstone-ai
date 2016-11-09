#pragma once

#include "Entity/Card.h"
#include "Manipulators/Helpers/BasicHelper.h"

namespace Manipulators
{
	class SpellManipulator
	{
	public:
		SpellManipulator(Entity::Card &card) : card_(card) {}

	private:
		Entity::Card & card_;
	};
}