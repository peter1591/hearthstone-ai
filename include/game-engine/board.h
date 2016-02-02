#ifndef GAME_ENGINE_BOARD_H
#define GAME_ENGINE_BOARD_H

#include <string.h>

#include <functional>
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
#include "next-move-getter.h"
#include "move.h"

namespace GameEngine {

class Board
{
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
			if (this == &rhs) return *this;

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
		void SetStateToPlayerTurnStart();
		void SetStateToPlayerChooseBoardMove();

		Stage GetStage() const { return this->stage; }
		StageType GetStageType() const { return (StageType)(this->stage & STAGE_TYPE_FLAG); }
		std::string GetStageName() const;

		// Return all possible moves
		// If this is a game flow node, you should skip this call
		// Note: caller should clear 'next_moves' before calling
		void GetNextMoves(NextMoveGetter &next_move_getter) const;

		void GetGoodMove(Move &next_move, unsigned int rand) const;

		// Apply the move to the board
		void ApplyMove(const Move &move);

		void DebugPrint() const;

		bool operator==(const Board &rhs) const;
		bool operator!=(const Board &rhs) const {
			return !(*this == rhs);
		}

	public: // internal state data for cross-stage communication
		struct PlayerPutMinionData {
			Hand::Locator hand_card;
			int location; // where to put the minion
			int required_target;
		};

		struct OpponentPutMinionData {
			Card card;
			int location;
		};

		struct PlayerAttackData {
			int attacker_idx;
			int attacked_idx;
		};

		struct OpponentAttackData {
			int attacker_idx;
			int attacked_idx;
		};

		union Data {
			PlayerPutMinionData player_put_minion_data;
			OpponentPutMinionData opponent_put_minion_data;
			PlayerAttackData player_attack_data;
			OpponentAttackData opponent_attack_data;

			Data() {}

			bool operator==(const Data &rhs) const = delete;
			bool operator!=(const Data &rhs) const = delete;
		};

	public:
		Stage stage;
		RandomGenerator random_generator;
		Data data;
};

inline bool Board::operator==(const Board &rhs) const
{
	if (this->player_stat != rhs.player_stat) return false;
	if (this->player_secrets != rhs.player_secrets) return false;
	if (this->player_hand != rhs.player_hand) return false;
	if (this->player_deck != rhs.player_deck) return false;
	if (this->player_minions != rhs.player_minions) return false;

	if (this->opponent_stat != rhs.opponent_stat) return false;
	if (this->opponent_secrets != rhs.opponent_secrets) return false;
	if (this->opponent_cards != rhs.opponent_cards) return false;
	if (this->opponent_minions != rhs.opponent_minions) return false;

	if (this->stage != rhs.stage) return false;
	if (this->random_generator != rhs.random_generator) return false;

	switch (this->stage) {
	case STAGE_PLAYER_PUT_MINION:
		if (this->data.player_put_minion_data.hand_card != rhs.data.player_put_minion_data.hand_card) return false;
		if (this->data.player_put_minion_data.location != rhs.data.player_put_minion_data.location) return false;
		if (this->data.player_put_minion_data.required_target != rhs.data.player_put_minion_data.required_target) return false;
		break;

	case GameEngine::STAGE_OPPONENT_PUT_MINION:
		if (this->data.opponent_put_minion_data.card != rhs.data.opponent_put_minion_data.card) return false;
		if (this->data.opponent_put_minion_data.location != rhs.data.opponent_put_minion_data.location) return false;
		break;

	case GameEngine::STAGE_PLAYER_ATTACK:
		if (this->data.player_attack_data.attacker_idx != rhs.data.player_attack_data.attacker_idx) return false;
		if (this->data.player_attack_data.attacked_idx != rhs.data.player_attack_data.attacked_idx) return false;
		break;

	case GameEngine::STAGE_OPPONENT_ATTACK:
		if (this->data.opponent_attack_data.attacker_idx != rhs.data.opponent_attack_data.attacker_idx) return false;
		if (this->data.opponent_attack_data.attacked_idx != rhs.data.opponent_attack_data.attacked_idx) return false;
		break;

	default:
		break;
	}

	return true;
}

} // namespace GameEngine

namespace std {
	template <> struct hash<GameEngine::Board> {
		typedef GameEngine::Board argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			GameEngine::hash_combine(result, hash<GameEngine::PlayerStat>()(s.player_stat));
			GameEngine::hash_combine(result, hash<GameEngine::Secrets>()(s.player_secrets));
			GameEngine::hash_combine(result, hash<GameEngine::Hand>()(s.player_hand));
			GameEngine::hash_combine(result, hash<GameEngine::Deck>()(s.player_deck));
			GameEngine::hash_combine(result, hash<GameEngine::Minions>()(s.player_minions));

			GameEngine::hash_combine(result, hash<GameEngine::PlayerStat>()(s.opponent_stat));
			GameEngine::hash_combine(result, hash<GameEngine::HiddenSecrets>()(s.opponent_secrets));
			GameEngine::hash_combine(result, hash<GameEngine::OpponentCards>()(s.opponent_cards));
			GameEngine::hash_combine(result, hash<GameEngine::Minions>()(s.opponent_minions));

			GameEngine::hash_combine(result, hash<GameEngine::Stage>()(s.stage));
			GameEngine::hash_combine(result, hash<GameEngine::RandomGenerator>()(s.random_generator));

			// hash for the union
			switch (s.stage) {
				case GameEngine::STAGE_PLAYER_PUT_MINION:
					GameEngine::hash_combine(result, hash<decltype(s.data.player_put_minion_data.hand_card)>()(s.data.player_put_minion_data.hand_card));
					GameEngine::hash_combine(result, hash<int>()(s.data.player_put_minion_data.location));
					GameEngine::hash_combine(result, hash<decltype(s.data.player_put_minion_data.required_target)>()(s.data.player_put_minion_data.required_target));
					break;

				case GameEngine::STAGE_OPPONENT_PUT_MINION:
					GameEngine::hash_combine(result, hash<GameEngine::Card>()(s.data.opponent_put_minion_data.card));
					GameEngine::hash_combine(result, hash<int>()(s.data.opponent_put_minion_data.location));
					break;

				case GameEngine::STAGE_PLAYER_ATTACK:
					GameEngine::hash_combine(result, hash<int>()(s.data.player_attack_data.attacker_idx));
					GameEngine::hash_combine(result, hash<int>()(s.data.player_attack_data.attacked_idx));
					break;

				case GameEngine::STAGE_OPPONENT_ATTACK:
					GameEngine::hash_combine(result, hash<int>()(s.data.opponent_attack_data.attacker_idx));
					GameEngine::hash_combine(result, hash<int>()(s.data.opponent_attack_data.attacked_idx));
					break;

				default:
					break;
			}

			return result;
		}
	};
}

#endif
