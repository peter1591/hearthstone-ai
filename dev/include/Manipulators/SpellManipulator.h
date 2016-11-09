#pragma once

#include "Entity/Card.h"
#include "Manipulators/Helpers/BasicHelper.h"

namespace Manipulators
{
	class SpellManipulator
	{
	public:
		SpellManipulator(Card &card) : card_(card) {}

	private:
		Card & card_;
	};
}