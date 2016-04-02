#ifndef GAME_ENGINE_BOARD_H
#define GAME_ENGINE_BOARD_H

#include <string.h>

#include <functional>
#include <list>

#include "game-engine/board-objects/object-manager.h"

#include "random-generator.h"
#include "common.h"
#include "stage.h"
#include "next-move-getter.h"
#include "move.h"
#include "player.h"
#include "hook-manager.h"

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

		bool operator==(const Board &rhs) const;
		bool operator!=(const Board &rhs) const;

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

	public: // debug
		void DebugPrint() const;

	public: // internal state data for cross-stage communication
		typedef Move::PlayHandMinionData PlayHandMinionData;
		typedef Move::PlayHandWeaponData PlayHandWeaponData;
		typedef Move::AttackData AttackData;

		union Data {
			PlayHandMinionData play_hand_minion_data;
			PlayHandWeaponData play_hand_weapon_data;
			AttackData attack_data;

			bool operator==(const Data &rhs) const = delete;
			bool operator!=(const Data &rhs) const = delete;
		};

	public:
		Player player;
		Player opponent;

		HookManager hook_manager;

		ObjectManager object_manager;

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

			GameEngine::hash_combine(result, s.player);
			GameEngine::hash_combine(result, s.opponent);

			GameEngine::hash_combine(result, s.stage);

			// hash for the union
			switch (s.stage) {
			case GameEngine::STAGE_PLAYER_PUT_MINION:
			case GameEngine::STAGE_OPPONENT_PUT_MINION:
				GameEngine::hash_combine(result, s.data.play_hand_minion_data);
				break;

			case GameEngine::STAGE_PLAYER_EQUIP_WEAPON:
			case GameEngine::STAGE_OPPONENT_EQUIP_WEAPON:
				GameEngine::hash_combine(result, s.data.play_hand_weapon_data);
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