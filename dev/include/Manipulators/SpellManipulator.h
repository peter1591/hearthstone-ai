#pragma once

#include "Manipulators/BasicManipulator.h"

namespace Manipulators
{
	class SpellManipulator : public BasicManipulator
	{
	public:
		SpellManipulator(Card &card) : BasicManipulator(card) {}
	};
}