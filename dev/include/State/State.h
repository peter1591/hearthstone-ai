#pragma once

#include "EntitiesManager/EntitiesManager.h"
#include "State/Players.h"
#include "State/ThisTurnCounters.h"

namespace State
{
	class State
	{
	public:
		void ChangePlayer()
		{
			if (current_player == kPlayerFirst) current_player = kPlayerSecond;
			else current_player = kPlayerFirst;
		}
		Player & GetCurrentPlayer() { return players.Get(current_player); }
		const Player & GetCurrentPlayer() const { return players.Get(current_player); }

	public:
		Players players;
		EntitiesManager mgr;

		PlayerIdentifier current_player;
		int turn;

		ThisTurnCounters this_turn_counters;
	};
}
