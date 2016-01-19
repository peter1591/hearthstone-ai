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
#include "stages/stage.h"

class Move
{
	public:
		enum {
			ACTION_UNKNOWN,
			ACTION_GAME_FLOW,
			ACTION_PLAY_HAND_CARD_MINION,
			ACTION_ATTACK,
			ACTION_END_TURN,
		} action;

		struct GameFlowData {
		};

		struct PlayHandCardMinionData {
			int idx_hand_card; // play the 'idx' hand card
			int location; // where to put the minion
		};

		struct AttackData {
			int attacking_idx; // 0 indicates the hero
			int attacked_idx; // 0 indicates the hero
		};

		struct ChoosePutMinionLocationData {
			size_t put_location;
		};

		union Data {
			GameFlowData game_flow_data;
			PlayHandCardMinionData play_hand_card_minion_data;
			AttackData attack_data;
			ChoosePutMinionLocationData choose_put_minion_location_data;

			Data() {}
		} data;

		Move() : action(ACTION_UNKNOWN) {}

		void DebugPrint() const;
};

class Board
{
	friend class StagePlayerTurnStart;
	friend class StagePlayerTurnEnd;
	friend class StagePlayerChooseBoardMove;

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
