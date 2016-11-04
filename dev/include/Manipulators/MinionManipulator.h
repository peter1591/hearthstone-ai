#pragma once

#include "Manipulators/BasicManipulator.h"

namespace Manipulators
{
	class MinionManipulator : public BasicManipulator
	{
	public:
		MinionManipulator(Card &card) : BasicManipulator(card) {}
	};
}