#ifndef BOARD_H
#define BOARD_H

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
#include "stages/stage.h"
#include "move.h"

class Board
{
	friend class StagePlayerTurnStart;
	friend class StagePlayerChooseBoardMove;
	friend class StagePlayerTurnEnd;
	friend class StageOpponentTurnStart;
	friend class STageOpponentChooseBoardMove;
	friend class STageOpponentTurnEnd;

	public:
		Board() : stage(STAGE_UNKNOWN) {}

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
		void SetStateToPlayerTurnStart();

		void GetStage(bool &is_player_turn, bool &is_random_node, Stage &stage) const;

		bool IsPlayerTurn() const;
		bool IsRandomNode() const;

		// Return all possible boards after a player/opponent's action 
		// If this is a random node, one of the random outcomes is returned
		void GetNextMoves(std::vector<Move> &next_moves) const;
		void ApplyMove(const Move &move);

		void DebugPrint() const;

	private: // internal state data for cross-stage communication
		union Data {
			Data() {}
			// TODO: implement comparison operator via memory-compare
		} data;

	private:
		Stage stage;
};

#endif
