#pragma once

#include "Entity/CardZone.h"

namespace Entity
{
	struct EnchantableStates
	{
		CardZone zone;
		int cost;
		int attack;
		int max_hp;
	};
}