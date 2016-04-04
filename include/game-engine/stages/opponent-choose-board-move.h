#ifndef STAGES_OPPONENT_CHOOSE_BOARD_MOVE_H
#define STAGES_OPPONENT_CHOOSE_BOARD_MOVE_H

#include <stdexcept>
#include <vector>

#include "game-engine/stages/common.h"
#include "game-engine/board.h"
#include "game-engine/next-move-getter.h"

namespace GameEngine {

class StageOpponentChooseBoardMove
{
public:
	static const Stage stage = STAGE_OPPONENT_CHOOSE_BOARD_MOVE;

	static void GetNextMoves(
		const Board &board, NextMoveGetter &next_move_getter, bool & is_deterministic)
	{
		StageHelper::GetBoardMoves(board.opponent, next_move_getter, is_deterministic);
	}

	static void GetGoodMove(Board const& board, Move &good_move, unsigned int rand)
	{
		StageHelper::GetGoodBoardMove(rand, board.opponent, good_move);
	}

	static void ApplyMove(Board &board, const Move &move)
	{
		switch (move.action)
		{
		case Move::ACTION_PLAY_HAND_MINION:
			board.data.play_hand_minion_data = move.data.play_hand_minion_data;
			board.stage = STAGE_OPPONENT_PUT_MINION;
			return;

		case Move::ACTION_PLAY_HAND_WEAPON:
			board.data.play_hand_card_data = move.data.play_hand_card_data;
			board.stage = STAGE_OPPONENT_EQUIP_WEAPON;
			return;

		case Move::ACTION_ATTACK:
			board.data.attack_data = move.data.attack_data;
			board.stage = STAGE_OPPONENT_ATTACK;
			return;

		case Move::ACTION_END_TURN:
			board.stage = STAGE_OPPONENT_TURN_END;
			return;

		default:
			throw std::runtime_error("Invalid action for StageOpponentChooseBoardMove");
		}
	}
};

} // namespace GameEngine

#endif
