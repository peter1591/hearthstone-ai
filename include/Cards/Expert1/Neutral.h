#pragma once

#include "Cards/id-map.h"
#include "Cards/MinionCardBase.h"
#include "FlowControl/Dispatchers/Minions.h"

namespace Cards
{
	class Card_EX1_089 : MinionCardBase
	{
	public:
		static void BattleCry(FlowControl::Context::BattleCry & context)
		{
			AnotherPlayer(context).GainEmptyCrystal();
		}
	};
}

REGISTER_MINION_CARD_CLASS(Cards::ID_EX1_089, Cards::Card_EX1_089)