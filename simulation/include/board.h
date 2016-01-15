#ifndef _BOARD_H
#define _BOARD_H

#include <list>

#include "common.h"
#include "card.h"
#include "deck.h"
#include "hand.h"
#include "secrets.h"
#include "minions.h"
#include "player-stat.h"
#include "hidden-deck.h"
#include "hidden-secrets.h"
#include "hidden-hand.h"

class BoardState
{
	public:
		enum Stage {
			STAGE_FLAG_RANDOM = 0x1000,
			STAGE_MASK_RANDOM = 0xF000,

			STAGE_UNKNOWN    = 0x0000,

			STAGE_START_TURN = 0x0001, // this should be the first stage for the player/opponent
			STAGE_DRAW       = 0x0002 | STAGE_FLAG_RANDOM,
			STAGE_END_TURN   = 0x0003 // this should be the last stage for the player/opponent
		};

	public:
		BoardState() : stage(STAGE_UNKNOWN) {}

		void SetPlayerTurn() { this->is_player_turn = true; }
		bool IsPlayerTurn() const { return this->is_player_turn; }

		bool IsRandomTurn() const { return this->stage & STAGE_MASK_RANDOM; }

		Stage GetStage() { return this->stage; }
		const Stage & GetStage() const { return this->stage; }
		void SetStage(Stage stage) { this->stage = stage; }

	private:
		bool is_player_turn;
		Stage stage;
};

class Move
{
	public:
		enum Action {
			ACTION_UNKNOWN,
			ACTION_STATE_TRANSFER
		};

		class StateTransferData {
			public:
				BoardState next_state;
		};

	public:
		Action action;

		union Data {
			StateTransferData state_transfer;

			Data() {}
		} data;

		void DebugPrint() const;
};

class Board
{
	public:
		Board();

		PlayerStat player_stat;
		Secrets player_secrets;
		Hand player_hand;
		Deck player_deck;
		Minions player_minions;

		PlayerStat opponent_stat;
		HiddenSecrets opponent_secrets;
		HiddenHand opponent_hand;
		HiddenDeck opponent_deck;
		Minions opponent_minions;

	public:
		void PlayerTurnStart() {
			// TODO: debug only
			this->state.SetStage(BoardState::STAGE_START_TURN);
			this->state.SetPlayerTurn();
		}

		const BoardState &GetState() const { return this->state; }

		std::list<Move> GetNextMoves();
		void ApplyMove(const Move &move);

		void DebugPrint() const;

	private:
		std::list<Move> DoStartTurn();
		std::list<Move> DoDraw();

	private:
		BoardState state;
};

inline Board::Board()
{
}

#endif
