#ifndef STAGES_PLAYER_TURN_END_H
#define STAGES_PLAYER_TURN_END_H

#include <stdexcept>
#include <vector>

#include "stages/common.h"
#include "board.h"

class StagePlayerTurnEnd
{
	public:
		static const Stage stage = STAGE_PLAYER_TURN_END;
		static std::string GetStageStringName() { return "StagePlayerTurnEnd"; }

		static void ApplyMove(Board &board, const Move &move)
		{
#ifdef DEBUG
			if (move.action != Move::ACTION_GAME_FLOW) throw std::runtime_error("Invalid move");
#endif

			// TODO: trigger end-turn actions

			board.stage = STAGE_OPPONENT_TURN_START;
		}
};

#endif
