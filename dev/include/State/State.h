#pragma once

#include "State/Players.h"
#include "EntitiesManager/EntitiesManager.h"

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
	};
}
