#ifndef _STAGES_END_TURN_H
#define _STAGES_END_TURN_H

#include <stdexcept>
#include <vector>

#include "stages/common.h"
#include "stages/start-turn.h"
#include "board.h"

class StageEndTurn
{
	public:
		static const bool is_random_node = StageStartTurn::is_random_node;

		static void GetNextMoves(const Board &board, std::vector<Move> &next_moves)
		{
			return StageStartTurn::GetNextMoves(board, next_moves);
		}

		static void ApplyMove(Board &board, const Move &move)
		{
			// TODO: change player/opponent
			return StageStartTurn::ApplyMove(board, move);
		}
};

#endif
