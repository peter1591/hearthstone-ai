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
		StageHelper::GetBoardMoves(board.player, next_move_getter, is_deterministic);
	}

	static void GetGoodMove(Board const& board, Move &good_move, unsigned int rand)
	{
		StageHelper::GetGoodBoardMove(rand, board.player, good_move);
	}

	static void ApplyMove(Board &board, const Move &move)
	{
		switch (move.action)
		{
			case Move::ACTION_PLAY_HAND_MINION:
				board.data.play_hand_minion_data = move.data.play_hand_minion_data;
				board.stage = STAGE_PLAYER_PUT_MINION;
				return;

			case Move::ACTION_PLAY_HAND_WEAPON:
				board.data.play_hand_card_data = move.data.play_hand_card_data;
				board.stage = STAGE_PLAYER_EQUIP_WEAPON;
				return;

			case Move::ACTION_PLAY_HAND_SPELL:
				board.data.play_hand_card_data = move.data.play_hand_card_data;
				board.stage = STAGE_PLAYER_PLAY_SPELL;
				return;

			case Move::ACTION_ATTACK:
				board.data.attack_data = move.data.attack_data;
				board.stage = STAGE_PLAYER_ATTACK;
				return;

			case Move::ACTION_HERO_POWER:
				board.stage = STAGE_PLAYER_USE_HERO_POWER;
				board.data.use_hero_power_data = move.data.use_hero_power_data;
				return;

			case Move::ACTION_END_TURN:
				board.stage = STAGE_PLAYER_TURN_END;
				return;

			default:
				throw std::runtime_error("Invalid action for StagePlayerChooseBoardMove");
		}
	}
};

} // namespace GameEngine

#endif
