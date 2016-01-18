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
			STAGE_UNKNOWN = 0,

			// player's or opponent's choice turn
			STAGE_CHOOSE_BOARD_MOVE, // play card from hand, minion attack, or end turn
//			STAGE_CHOOSE_HIDDEN_SECRET, // only for opponent
//			STAGE_CHOOSE_DISCOVER_CARD,

			// game flow including randoms
			// treated as random nodes in MCTS
			STAGE_START_TURN,
			STAGE_END_TURN,

			STAGE_WIN,
			STAGE_LOSS
		};

	public:
		BoardState() : stage(STAGE_UNKNOWN) {}

		inline void Set(bool is_player_turn, Stage stage)
		{
			this->is_player_turn = is_player_turn;
			this->stage = stage;
		}

		inline void Set(Stage stage)
		{
			this->Set(this->is_player_turn, stage);
		}

		inline bool IsPlayerTurn() const { return this->is_player_turn; }
		bool IsRandomNode() const;
		inline const Stage & GetStage() const { return this->stage; }

	private:
		bool is_player_turn;
		Stage stage;
};

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

		BoardState next_state;

		Move() : action(ACTION_UNKNOWN) {}

		void DebugPrint() const;
};

class Board
{
	friend class StageStartTurn;
	friend class StageEndTurn;
	friend class StageChooseBoardMove;
	friend class StageChoosePutMinionLocation;

	public:
		Board() {}

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
			this->state.Set(true, BoardState::STAGE_START_TURN);
		}

		const BoardState &GetState() const { return this->state; }
		BoardState &GetState() { return this->state; }

		// Return all possible boards after a player/opponent's action 
		// If this is a random node, one of the random outcomes is returned
		void GetNextMoves(std::vector<Move> &next_moves) const;
		void ApplyMove(const Move &move);

		void DebugPrint() const;

	private: // internal state data
		/// the data to put a minion from hand to board
		struct PlayMinionData {
			std::vector<Card>::iterator it_hand_card; // the iterator to the playing card within hand
			int put_location; // 0 indicates the leftmost
		};

		union Data {
			PlayMinionData play_minion_data;

			Data() {}
			// TODO: implement comparison operator via memory-compare
		} data;

	private:
		BoardState state;
};

#endif
