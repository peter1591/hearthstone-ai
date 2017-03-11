#pragma once

#include "state/Cards/Manager.h"
#include "state/board/Board.h"
#include "state/Events/Manager.h"

namespace state
{
	class State
	{
	public:
		board::Player & GetCurrentPlayer() { return board.Get(current_player); }
		const board::Player & GetCurrentPlayer() const { return board.Get(current_player); }

		board::Player & GetOppositePlayer() { return board.Get(current_player.Opposite()); }
		const board::Player & GetOppositePlayer() const { return board.Get(current_player.Opposite()); }

	public:
		board::Board board;
		Cards::Manager mgr;
		Events::Manager event_mgr;

		PlayerIdentifier current_player;
		int turn;
	};
}
