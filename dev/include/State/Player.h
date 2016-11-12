#pragma once

#include "State/Hand.h"
#include "State/Minions.h"
#include "State/Weapon.h"

namespace State
{
	class Player
	{
	public:
		Hand hand_;
		Minions minions_;
		Weapon weapon_;
	};
}