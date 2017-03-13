#pragma once

#include "state/Cards/Manager.h"
#include "state/board/Board.h"
#include "state/Events/Manager.h"

namespace state
{
	class State
	{
	public:
		PlayerIdentifier const& GetCurrentPlayerId() const { return current_player_; }
		PlayerIdentifier & GetMutableCurrentPlayerId() { return current_player_; }

		int GetTurn() const { return turn_; }
		void SetTurn(int turn) { turn = turn_; }
		void IncreaseTurn() { ++turn_; }

	public:
		board::Player & GetCurrentPlayer() { return board.Get(current_player_); }
		const board::Player & GetCurrentPlayer() const { return board.Get(current_player_); }

		board::Player & GetOppositePlayer() { return board.Get(current_player_.Opposite()); }
		const board::Player & GetOppositePlayer() const { return board.Get(current_player_.Opposite()); }

	public: // mutate functions
		// push new card --> card_ref
		// change card_ref zone
		// trigger events
		// set card cost/attack/hp/etc.

	public:
		board::Board board;
		Cards::Manager mgr;
		Events::Manager event_mgr;

	private:
		PlayerIdentifier current_player_;
		int turn_;
	};
}
