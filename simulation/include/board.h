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
			STAGE_UNKNOWN    = 0x0000,

			STAGE_START_TURN = 0x0001, // this should be the first stage for the player/opponent
			STAGE_DRAW       = 0x0002,
			STAGE_END_TURN   = 0x0003 // this should be the last stage for the player/opponent
		};

	public:
		BoardState() : stage(STAGE_UNKNOWN) {}

	private:
		bool IsPlayerTurn() { return this->is_player_turn; }
		bool IsRandomTurn() { return this->is_random_turn; }
		Stage GetStage() { return this->stage; }
		void SetStage(Stage stage) { this->stage = stage; }

	private:
		bool is_player_turn;
		bool is_random_turn;
		Stage stage;
};

class Move
{
	public:
		BoardState next_state;
	// TODO
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
		void SetStateToPlayerTurn(); // TODO: debug only

		std::list<Move> GetNextMoves();
		void ApplyMove(const Move &move);

		void DebugPrint() const;

	public:
		inline bool IsPlayerTurn() { return this->state & STATE_FLAG_PLAYER_TURN; }
		inline bool IsOpponentTurn() { return !this->IsPlayerTurn(); }
		inline bool IsRandomTurn() { return this->state & STATE_FLAG_RANDOM_TURN; }

	private:

	private:
		State state;
};

inline Board::Board()
{
	this->state = STATE_UNKNOWN;
}

inline void Board::SetStateToPlayerTurn()
{
	this->state = (Board::State)(Board::STATE_START_TURN | Board::STATE_FLAG_PLAYER_TURN);
}

#endif
