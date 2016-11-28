#ifndef STAGES_PLAYER_TURN_START_H
#define STAGES_PLAYER_TURN_START_H

#include <stdexcept>
#include <vector>
#include <string>

#include "game-engine/stages/common.h"
#include "game-engine/stages/helper.h"
#include "game-engine/board.h"

namespace GameEngine {

class StagePlayerTurnStart
{
	public:
		static const Stage stage = STAGE_PLAYER_TURN_START;

		static void Go(Board &board)
		{
			if (StageHelper::PlayerDrawCard(board.player)) return;
			board.hook_manager.PlayerTurnStart();
			board.stage = STAGE_PLAYER_CHOOSE_BOARD_MOVE;
	}
};

} // namespace GameEngine

#endif
