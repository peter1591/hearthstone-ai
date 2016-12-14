#pragma once

#include "State/Types.h"

namespace Cards
{
	struct CardData
	{
		int card_id;
		state::CardType card_type;
		// TODO: race

		int cost;
		int attack;
		int max_hp;
	};
}