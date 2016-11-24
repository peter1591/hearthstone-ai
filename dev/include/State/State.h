#pragma once

#include "EntitiesManager/EntitiesManager.h"
#include "EventManager/HandlersManager.h"
#include "State/Board/Board.h"
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
		Player & GetCurrentPlayer() { return board.players.Get(current_player); }
		const Player & GetCurrentPlayer() const { return board.players.Get(current_player); }

	public:
		Board board;
		EntitiesManager mgr;
		EventManager::HandlersManager event_mgr;

		PlayerIdentifier current_player;
		int turn;

		ThisTurnCounters this_turn_counters;
	};
}
