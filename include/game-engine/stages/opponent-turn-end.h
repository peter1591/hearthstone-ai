#ifndef STAGES_OPPONENT_TURN_END_H
#define STAGES_OPPONENT_TURN_END_H

#include <stdexcept>
#include <vector>

#include "game-engine/stages/common.h"
#include "game-engine/board.h"

namespace GameEngine {

class StageOpponentTurnEnd
{
	public:
		static const Stage stage = STAGE_OPPONENT_TURN_END;

		static void Go(Board &board)
		{
			// TODO: trigger end-turn actions
			board.player_minions.TurnEnd();
			board.opponent_minions.TurnEnd();

			board.stage = STAGE_PLAYER_TURN_START;
		}
};

} // namespace GameEngine

#endif
