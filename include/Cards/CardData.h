#pragma once

#include "State/Types.h"

namespace Cards
{
	struct CardData
	{
		int card_id;
		state::CardType card_type;
		state::CardRace card_race;

		int cost;
		int attack;
		int max_hp;

		static constexpr int kFieldChangeId = 1; // modify this if any field changed. This helps to track which codes should be modified accordingly.
	};
}