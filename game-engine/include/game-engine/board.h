#ifndef GAME_ENGINE_BOARD_H
#define GAME_ENGINE_BOARD_H

#include <string.h>

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
#include "stage.h"
#include "move.h"

namespace GameEngine {

class Board
{
	friend class StageHelper;
	friend class StagePlayerTurnStart;
	friend class StagePlayerChooseBoardMove;
	friend class StagePlayerPutMinion;
	friend class StagePlayerAttack;
	friend class StagePlayerTurnEnd;
	friend class StageOpponentTurnStart;
	friend class StageOpponentChooseBoardMove;
	friend class StageOpponentPutMinion;
	friend class StageOpponentAttack;
	friend class StageOpponentTurnEnd;

	public:
		Board() :
			player_deck(&this->random_generator),
			stage(STAGE_UNKNOWN)
		{
		}

		Board(const Board &rhs) : player_deck(&this->random_generator) {
			this->operator=(rhs);
		}

		Board & operator=(const Board &rhs) {
			this->player_stat = rhs.player_stat;
			this->player_secrets = rhs.player_secrets;
			this->player_hand = rhs.player_hand;
			this->player_deck.Assign(rhs.player_deck, &this->random_generator);
			this->player_minions = rhs.player_minions;
			this->opponent_stat = rhs.opponent_stat;
			this->opponent_secrets = rhs.opponent_secrets;
			this->opponent_cards = rhs.opponent_cards;
			this->opponent_minions = rhs.opponent_minions;

			this->stage = rhs.stage;
			this->random_generator = rhs.random_generator;
			this->data = rhs.data;

			return *this;
		}

	public:
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
		void Initialize(unsigned int rand_seed);

		void SetStateToPlayerTurnStart();

		Stage GetStage() const { return this->stage; }
		StageType GetStageType() const { return (StageType)(this->stage & STAGE_TYPE_FLAG); }
		std::string GetStageName() const;

		bool IsPlayerTurn() const;
		bool IsRandomNode() const;

		// Return all possible moves
		// If this is a game flow node, you should skip this call
		// Note: caller should clear 'next_moves' before calling
		void GetNextMoves(std::vector<Move> &next_moves) const;

		// Apply the move to the board
		// @params   is_deterministic   [OUT] is the apply procedure deterministic (.e., introduce no random?)
		void ApplyMove(const Move &move, bool &is_deterministic);

		void DebugPrint() const;

		bool operator==(const Board &rhs) const;
		bool operator!=(const Board &rhs) const {
			return !(*this == rhs);
		}

	private: // internal state data for cross-stage communication
		struct PlayerPutMinionData {
			int idx_hand_card; // play the 'idx' hand card
			int location; // where to put the minion
		};

		struct OpponentPutMinionData {
			Card card;
			int location;
		};

		struct PlayerAttackData {
			int attacker_idx; // -1 for the hero
			int attacked_idx; // -1 for the hero
		};

		struct OpponentAttackData {
			int attacker_idx; // -1 for the hero
			int attacked_idx; // -1 for the hero
		};

		union Data {
			PlayerPutMinionData player_put_minion_data;
			OpponentPutMinionData opponent_put_minion_data;
			PlayerAttackData player_attack_data;
			OpponentAttackData opponent_attack_data;

			Data() {}

			bool operator==(const Data &rhs) const {
				if (memcmp(this, &rhs, sizeof(Data)) == 0) return true;
				return false;
			}

			bool operator!=(const Data &rhs) const {
				return !(*this == rhs);
			}
		};

	private:
		Stage stage;
		RandomGenerator random_generator;
		Data data;
};

} // namespace GameEngine

#endif
