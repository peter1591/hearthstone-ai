#ifndef STAGES_OPPONENT_TURN_START_H
#define STAGES_OPPONENT_TURN_START_H

#include <stdexcept>
#include <vector>
#include <string>

#include "game-engine/stages/common.h"
#include "game-engine/stages/helper.h"
#include "game-engine/board.h"

namespace GameEngine {

class StageOpponentTurnStart
{
	public:
		static const Stage stage = STAGE_OPPONENT_TURN_START;

		static void Go(Board &board)
		{
			if (StageHelper::PlayerDrawCard(board.opponent)) return;

			board.opponent.stat.crystal.TurnStart();
			board.object_manager.OpponentTurnStart();

			board.stage = STAGE_OPPONENT_CHOOSE_BOARD_MOVE;
	}
};

} // namespace GameEngine

#endif
