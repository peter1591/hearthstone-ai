#pragma once

#include "State/Cards/Manager.h"
#include "State/board/Board.h"
#include "State/Events/Manager.h"

namespace state
{
	class State
	{
	public:
		void ChangePlayer()
		{
			if (current_player == kPlayerFirst) current_player = kPlayerSecond;
			else current_player = kPlayerFirst;
		}
		board::Player & GetCurrentPlayer() { return board.players.Get(current_player); }
		const board::Player & GetCurrentPlayer() const { return board.players.Get(current_player); }

	public:
		board::Board board;
		Cards::Manager mgr;
		Events::Manager event_mgr;

		PlayerIdentifier current_player;
		int turn;
	};
}
