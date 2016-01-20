#ifndef BOARD_H
#define BOARD_H

#include <list>

#include "random-generator.h"
#include "common.h"
#include "card.h"
#include "deck.h"
#include "hand.h"
#include "secrets.h"
#include "minions.h"
#include "player-stat.h"
#include "opponent-cards.h"
#include "hidden-secrets.h"
#include "stages/stage.h"
#include "move.h"

class Board
{
	friend class StagePlayerTurnStart;
	friend class StagePlayerChooseBoardMove;
	friend class StagePlayerPutMinion;
	friend class StagePlayerTurnEnd;
	friend class StageOpponentTurnStart;
	friend class StageOpponentChooseBoardMove;
	friend class StageOpponentPutMinion;
	friend class StageOpponentTurnEnd;

	public:
		Board() :
			player_deck(this->random_generator),
			stage(STAGE_UNKNOWN)
		{}

		PlayerStat player_stat;
		Secrets player_secrets;
		Hand player_hand;
		Deck player_deck;
		Minions player_minions;

		PlayerStat opponent_stat;
		HiddenSecrets opponent_secrets;
		OpponentCards opponent_cards;
		Minions opponent_minions;

	public:
		void SetStateToPlayerTurnStart();

		void GetStage(Stage &stage, StageType &type) const;

		bool IsPlayerTurn() const;
		bool IsRandomNode() const;

		// Return all possible boards after a player/opponent's action 
		// If this is a random node, one of the random outcomes is returned
		void GetNextMoves(std::vector<Move> &next_moves) const;

		// Apply the move to the board
		// @params   is_deterministic   [OUT] is the apply procedure deterministic (.e., introduce no random?)
		void ApplyMove(const Move &move, bool &is_deterministic);

		void DebugPrint() const;

	private: // internal state data for cross-stage communication
		struct PlayerPutMinionData {
			int idx_hand_card; // play the 'idx' hand card
			int location; // where to put the minion
		};

		struct OpponentPutMinionData {
			Card card;
			int location;
		};

		union Data {
			PlayerPutMinionData player_put_minion_data;
			OpponentPutMinionData opponent_put_minion_data;

			Data() {}
			// TODO: implement comparison operator via memory-compare
		} data;

	private:
		Stage stage;
		RandomGenerator random_generator;
};

#endif
