#ifndef STAGES_OPPONENT_TURN_START_H
#define STAGES_OPPONENT_TURN_START_H

#include <stdexcept>
#include <vector>
#include <string>

#include "stages/common.h"
#include "stages/helper.h"
#include "game-engine/board.h"

namespace GameEngine {

class StageOpponentTurnStart
{
	public:
		static const Stage stage = STAGE_OPPONENT_TURN_START;

		static void Go(Board &board)
		{
			board.opponent_stat.crystal.TurnStart();

			if (StageHelper::OpponentDrawCard(board)) return;

			// reset minion stat
			for (auto &minion : board.opponent_minions.GetMinions()) {
				minion.TurnStart();
			}

			board.stage = STAGE_OPPONENT_CHOOSE_BOARD_MOVE;
	}
};

} // namespace GameEngine

#endif
