#pragma once

#include "state/Cards/Manager.h"
#include "state/board/Board.h"
#include "state/Events/Manager.h"

namespace state
{
	class State
	{
	public:
		PlayerIdentifier const& GetCurrentPlayerId() const { return current_player; }
		PlayerIdentifier & GetMutableCurrentPlayerId() { return current_player; }

	public:
		board::Player & GetCurrentPlayer() { return board.Get(current_player); }
		const board::Player & GetCurrentPlayer() const { return board.Get(current_player); }

		board::Player & GetOppositePlayer() { return board.Get(current_player.Opposite()); }
		const board::Player & GetOppositePlayer() const { return board.Get(current_player.Opposite()); }

	public: // mutate functions
		// push new card --> card_ref
		// change card_ref zone
		// trigger events
		// set card cost/attack/hp/etc.

	public:
		board::Board board;
		Cards::Manager mgr;
		Events::Manager event_mgr;

		int turn;

	private:
		PlayerIdentifier current_player;
	};
}
