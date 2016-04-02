#ifndef STAGES_PLAYER_TURN_END_H
#define STAGES_PLAYER_TURN_END_H

#include <stdexcept>
#include <vector>

#include "game-engine/stages/common.h"
#include "game-engine/board.h"

namespace GameEngine {

class StagePlayerTurnEnd
{
	public:
		static const Stage stage = STAGE_PLAYER_TURN_END;

		static void Go(Board &board)
		{
			board.hook_manager.PlayerTurnEnd();

			// TODO: trigger end-turn actions
			board.stage = STAGE_OPPONENT_TURN_START;
		}
};

} // namespace GameEngine

#endif
