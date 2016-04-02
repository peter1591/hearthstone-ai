#ifndef STAGES_PLAYER_CHOOSE_BOARD_MOVE_H
#define STAGES_PLAYER_CHOOSE_BOARD_MOVE_H

#include <stdexcept>
#include <vector>
#include <memory>

#include "game-engine/stages/common.h"
#include "game-engine/stages/weighted-moves.h"
#include "game-engine/board.h"
#include "game-engine/next-move-getter.h"
#include "game-engine/cards/common.h"

namespace GameEngine {

class StagePlayerChooseBoardMove
{
public:
	static const Stage stage = STAGE_PLAYER_CHOOSE_BOARD_MOVE;

	static void GetNextMoves(
		const Board &board, NextMoveGetter &next_move_getter, bool & is_deterministic)
	{
		StageHelper::GetBoardMoves(board, board.player, next_move_getter, is_deterministic);
	}

	static void GetGoodMove(Board const& board, Move &good_move, unsigned int rand)
	{
		StageHelper::GetGoodBoardMove(rand, board, board.player, good_move);
	}

	static void ApplyMove(Board &board, const Move &move)
	{
		switch (move.action)
		{
			case Move::ACTION_PLAY_HAND_MINION:
				return StagePlayerChooseBoardMove::PlayMinion(board, move);

			case Move::ACTION_PLAY_HAND_WEAPON:
				return StagePlayerChooseBoardMove::EquipWeapon(board, move);

			case Move::ACTION_ATTACK:
				return StagePlayerChooseBoardMove::PlayerAttack(board, move);

			case Move::ACTION_END_TURN:
				return StagePlayerChooseBoardMove::EndTurn(board, move);

			default:
				throw std::runtime_error("Invalid action for StagePlayerChooseBoardMove");
		}
	}

private:
	static void PlayMinion(Board &board, const Move &move)
	{
		board.data.play_hand_minion_data = move.data.play_hand_minion_data;
		board.stage = STAGE_PLAYER_PUT_MINION;
	}

	static void EquipWeapon(Board &board, const Move &move)
	{
		board.data.play_hand_weapon_data = move.data.play_hand_weapon_data;
		board.stage = STAGE_PLAYER_EQUIP_WEAPON;
	}

	static void PlayerAttack(Board &board, const Move &move)
	{
		board.data.attack_data = move.data.attack_data;
		board.stage = STAGE_PLAYER_ATTACK;
	}

	static void EndTurn(Board &board, const Move &)
	{
		board.stage = STAGE_PLAYER_TURN_END;
	}
};

} // namespace GameEngine

#endif
