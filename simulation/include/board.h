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
			STAGE_CHOOSE_PUT_MINION_LOCATION,
			STAGE_CHOOSE_HIDDEN_SECRET, // only for opponent
			STAGE_CHOOSE_DISCOVER_CARD,

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

			switch (stage) {
				case STAGE_CHOOSE_BOARD_MOVE:
				case STAGE_CHOOSE_HIDDEN_SECRET:
				case STAGE_CHOOSE_DISCOVER_CARD:
				case STAGE_CHOOSE_PUT_MINION_LOCATION:
					this->is_random_node = false;
					break;

				case STAGE_START_TURN:
				case STAGE_END_TURN:
				case STAGE_WIN:
				case STAGE_LOSS:
					this->is_random_node = true;
					break;

				case STAGE_UNKNOWN:
					throw std::runtime_error("Invalid argument");
					break;

			}
		}

		inline void Set(Stage stage)
		{
			this->Set(this->is_player_turn, stage);
		}

		inline bool IsPlayerTurn() const { return this->is_player_turn; }
		inline bool IsRandomTurn() const { return this->is_random_node; }
		inline const Stage & GetStage() const { return this->stage; }

	private:
		bool is_player_turn;
		bool is_random_node;
		Stage stage;
};

class Move
{
	public:
		enum {
			ACTION_UNKNOWN,
			ACTION_GAME_FLOW,
			ACTION_PLAY_HAND_CARD,
			ACTION_ATTACK,
			ACTION_END_TURN,
			ACTION_CHOOSE_PUT_MINION_LOCATION
		} action;

		struct GameFlowData {
		};

		struct PlayHandCardData {
			int idx; // play the 'idx' hand card
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
			PlayHandCardData play_hand_card_data;
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
		void Go(); // do game flow

		void DoStartTurn();
		void DoPlayHandCard(const Move::PlayHandCardData &data);
		void DoEndTurn();
		void DoChoosePutMinionLocation(const Move::ChoosePutMinionLocationData &data);

		void GetGameFlowMove(std::vector<Move> &moves) const;
		void GetBoardMoves(std::vector<Move> &moves) const;
		void GetPutMinionLocationMoves(std::vector<Move> &moves) const;

	private:
		BoardState state;
		Move last_move;
};

inline Board::Board()
{
}

#endif
