#ifndef GAME_ENGINE_BOARD_H
#define GAME_ENGINE_BOARD_H

#include <string.h>

#include <functional>
#include <list>

#include "game-engine/board-objects/object-manager.h"

#include "random-generator.h"
#include "common.h"
#include "card.h"
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
		Board();

	private: // deep copy; marked as private, so caller need to call Clone()
		Board(const Board &rhs);
		Board & operator=(const Board &rhs) = delete;
	public:
		static Board Clone(Board const& rhs);
		Board(Board && rhs);
		Board & operator=(Board && rhs);

	public:
		PlayerStat player_stat;
		Secrets player_secrets;
		Hand player_hand;

		PlayerStat opponent_stat;
		HiddenSecrets opponent_secrets;
		OpponentCards opponent_cards;

		BoardObjects::ObjectManager object_manager;

	public:
		void SetRandomSeed(unsigned int random_seed);
		void SetStateToPlayerChooseBoardMove();

		Stage GetStage() const { return this->stage; }
		StageType GetStageType() const { return (StageType)(this->stage & STAGE_TYPE_FLAG); }
		std::string GetStageName() const;

		// Return all possible moves
		void GetNextMoves(NextMoveGetter &next_move_getter, bool & is_deterministic) const;
		void GetNextMoves(GameEngine::Move & next_move, bool & is_deterministic) const;

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

	private:
		int random_seed;
	};

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

#endif