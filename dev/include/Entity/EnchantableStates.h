#pragma once

#include "Entity/CardZone.h"
#include "State/PlayerIdentifier.h"

namespace Entity
{
	class EnchantableStates
	{
	public:
		EnchantableStates() : zone(kCardZoneInvalid), cost(-1), attack(-1), max_hp(-1) {}

		State::PlayerIdentifier player;
		CardZone zone;
		int cost;
		int attack;
		int max_hp;
	};
}