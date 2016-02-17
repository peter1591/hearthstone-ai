#ifndef GAME_ENGINE_BOARD_H
#define GAME_ENGINE_BOARD_H

#include <string.h>

#include <functional>
#include <list>

#include "game-engine/board-objects/object-manager.h"

#include "random-generator.h"
#include "common.h"
#include "card.h"
#include "deck.h"
#include "hand.h"
#include "secrets.h"
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
			player_deck(this->random_generator),
			object_manager(*this),
			stage(STAGE_UNKNOWN)
		{
		}

	private: // deep copy; marked as private, so caller need to call Clone()
		Board(const Board &rhs) :
			player_deck(this->random_generator),
			object_manager(*this, rhs.object_manager)
		{
			this->player_stat = rhs.player_stat;
			this->player_secrets = rhs.player_secrets;
			this->player_hand = rhs.player_hand;
			this->player_deck = rhs.player_deck;
			this->opponent_stat = rhs.opponent_stat;
			this->opponent_secrets = rhs.opponent_secrets;
			this->opponent_cards = rhs.opponent_cards;

			this->stage = rhs.stage;
			this->random_generator = rhs.random_generator;
			this->data = rhs.data;
		}
		Board & operator=(const Board &rhs) = delete;

	public:
		static Board Clone(Board const& rhs) {
			return Board(rhs);
		}

		Board(Board && rhs) : player_deck(this->random_generator), object_manager(*this)
		{
			*this = std::move(rhs); 
		}
		Board & operator=(Board && rhs) {
			if (this != &rhs) {
				this->player_stat = std::move(rhs.player_stat);
				this->player_secrets = std::move(rhs.player_secrets);
				this->player_hand = std::move(rhs.player_hand);
				this->player_deck = std::move(rhs.player_deck);
				this->opponent_stat = std::move(rhs.opponent_stat);
				this->opponent_secrets = std::move(rhs.opponent_secrets);
				this->opponent_cards = std::move(rhs.opponent_cards);

				this->object_manager = std::move(rhs.object_manager);

				this->stage = std::move(rhs.stage);
				this->random_generator = std::move(rhs.random_generator);
				this->data = std::move(rhs.data);
			}

			return *this;
		}

	public:
		PlayerStat player_stat;
		Secrets player_secrets;
		Hand player_hand;
		Deck player_deck;

		PlayerStat opponent_stat;
		HiddenSecrets opponent_secrets;
		OpponentCards opponent_cards;

		BoardObjects::ObjectManager object_manager;

	public:
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
		// @param move  [IN] the move to apply on the board
		// @param introduced_random [OUT] does the move application introduced any random?
		void ApplyMove(const Move &move, bool * introduced_random = nullptr);

		void DebugPrint() const;

		bool operator==(const Board &rhs) const;
		bool operator!=(const Board &rhs) const;

	public: // internal state data for cross-stage communication

		typedef Move::PlayerPlayMinionData PlayerPlayMinionData;
		typedef Move::OpponentPlayMinionData OpponentPlayMinionData;
		typedef Move::PlayerEquipWeaponData PlayerEquipWeaponData;
		typedef Move::OpponentEquipWeaponData OpponentEquipWeaponData;
		typedef Move::AttackData AttackData;

		union Data {
			PlayerPlayMinionData player_play_minion_data;
			OpponentPlayMinionData opponent_play_minion_data;
			PlayerEquipWeaponData player_equip_weapon_data;
			OpponentEquipWeaponData opponent_equip_weapon_data;
			AttackData attack_data;

			bool operator==(const Data &rhs) const = delete;
			bool operator!=(const Data &rhs) const = delete;
		};

	public:
		Stage stage;
		RandomGenerator random_generator;
		Data data;

	private:
		template<typename Chooser, typename... Params>
		static typename Chooser::ReturnType StageFunctionCaller(Stage const stage, Params && ...params);
};

inline bool Board::operator==(const Board &rhs) const
{
	if (this->stage != rhs.stage) return false;

	if (this->player_stat != rhs.player_stat) return false;
	if (this->player_secrets != rhs.player_secrets) return false;
	if (this->player_hand != rhs.player_hand) return false;
	if (this->player_deck != rhs.player_deck) return false;

	if (this->opponent_stat != rhs.opponent_stat) return false;
	if (this->opponent_secrets != rhs.opponent_secrets) return false;
	if (this->opponent_cards != rhs.opponent_cards) return false;

	if (this->object_manager != rhs.object_manager) return false;

	switch (this->stage) {
	case STAGE_PLAYER_PUT_MINION:
		if (this->data.player_play_minion_data != rhs.data.player_play_minion_data) return false;
		break;

	case STAGE_OPPONENT_PUT_MINION:
		if (this->data.opponent_play_minion_data != rhs.data.opponent_play_minion_data) return false;
		break;

	case STAGE_PLAYER_EQUIP_WEAPON:
		if (this->data.player_equip_weapon_data != rhs.data.player_equip_weapon_data) return false;
		break;

	case STAGE_OPPONENT_EQUIP_WEAPON:
		if (this->data.opponent_equip_weapon_data != rhs.data.opponent_equip_weapon_data) return false;
		break;

	case STAGE_PLAYER_ATTACK:
	case STAGE_OPPONENT_ATTACK:
		if (this->data.attack_data != rhs.data.attack_data) return false;
		break;

	default:
		break;
	}

	return true;
}

inline bool Board::operator!=(Board const& rhs) const
{
	return !(*this == rhs);
}

} // namespace GameEngine

namespace std {
	template <> struct hash<GameEngine::Board> {
		typedef GameEngine::Board argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			GameEngine::hash_combine(result, s.player_stat);
			GameEngine::hash_combine(result, s.player_secrets);
			GameEngine::hash_combine(result, s.player_hand);
			GameEngine::hash_combine(result, s.player_deck);

			GameEngine::hash_combine(result, s.opponent_stat);
			GameEngine::hash_combine(result, s.opponent_secrets);
			GameEngine::hash_combine(result, s.opponent_cards);

			GameEngine::hash_combine(result, s.object_manager);

			GameEngine::hash_combine(result, s.stage);

			// hash for the union
			switch (s.stage) {
				case GameEngine::STAGE_PLAYER_PUT_MINION:
					GameEngine::hash_combine(result, s.data.player_play_minion_data);
					break;

				case GameEngine::STAGE_OPPONENT_PUT_MINION:
					GameEngine::hash_combine(result, s.data.opponent_play_minion_data);
					break;

				case GameEngine::STAGE_PLAYER_EQUIP_WEAPON:
					GameEngine::hash_combine(result, s.data.player_equip_weapon_data);
					break;

				case GameEngine::STAGE_OPPONENT_EQUIP_WEAPON:
					GameEngine::hash_combine(result, s.data.opponent_equip_weapon_data);
					break;

				case GameEngine::STAGE_PLAYER_ATTACK:
				case GameEngine::STAGE_OPPONENT_ATTACK:
					GameEngine::hash_combine(result, s.data.attack_data);
					break;

				default:
					break;
			}

			return result;
		}
	};
}

#include "game-engine/board-objects/minion-manipulator-impl.h"
#include "game-engine/board-objects/minions-impl.h"

#endif