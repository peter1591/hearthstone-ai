#pragma once

#include "State/Minions.h"
#include "State/Weapon.h"

namespace State
{
	class Player
	{
	public:
		Minions minions_;
		Weapon weapon_;
	};
}