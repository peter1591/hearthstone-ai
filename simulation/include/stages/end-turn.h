#ifndef _STAGES_END_TURN_H
#define _STAGES_END_TURN_H

#include <stdexcept>
#include <vector>

#include "stages/common.h"
#include "stages/player-turn-start.h"
#include "board.h"

class StageEndTurn
{
	public:
		static const bool is_random_node = StagePlayerTurnStart::is_random_node;
		static const bool is_player_turn = StagePlayerTurnStart::is_player_turn;

		static void GetNextMoves(const Board &board, std::vector<Move> &next_moves)
		{
			return StagePlayerTurnStart::GetNextMoves(board, next_moves);
		}

		static void ApplyMove(Board &board, const Move &move)
		{
			// TODO: change player/opponent
			return StagePlayerTurnStart::ApplyMove(board, move);
		}
};

#endif
