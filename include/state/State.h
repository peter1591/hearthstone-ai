#pragma once

#include "state/Cards/Manager.h"
#include "state/board/Board.h"
#include "state/Events/Manager.h"

namespace state
{
	class State
	{
	public:
		Cards::Manager const& GetCardsManager() const { return cards_mgr_; }
		Cards::Manager & GetCardsManager() { return cards_mgr_; }

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
		Events::Manager event_mgr;

	private:
		Cards::Manager cards_mgr_;

		PlayerIdentifier current_player_;
		int turn_;
	};
}
