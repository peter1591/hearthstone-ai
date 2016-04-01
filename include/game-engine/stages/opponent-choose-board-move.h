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

		static void GetNextMoves(const Board &board, NextMoveGetter &next_move_getter, bool & is_deterministic)
		{
			// TODO: implement
			throw std::runtime_error("not implemented");
		}

		static void GetGoodMove(Board const& board, Move &good_move, unsigned int rand)
		{
			// TODO: implement
			throw std::runtime_error("not implemented");
		}

		static void ApplyMove(Board &board, const Move &move)
		{
			// TODO: implement
			throw std::runtime_error("not implemented");
		}
};

} // namespace GameEngine

#endif
