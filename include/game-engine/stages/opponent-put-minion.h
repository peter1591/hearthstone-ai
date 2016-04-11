#ifndef STAGES_OPPONENT_PUT_MINION_H
#define STAGES_OPPONENT_PUT_MINION_H

#include <stdexcept>
#include <vector>

#include "game-engine/stages/common.h"
#include "game-engine/board.h"

namespace GameEngine {

class StageOpponentPutMinion
{
	public:
		static const Stage stage = STAGE_OPPONENT_PUT_MINION;

		static void Go(Board &board)
		{
			if (StageHelper::PlayMinion(board.opponent, board.data.play_hand_minion_data)) return;
			board.stage = STAGE_OPPONENT_CHOOSE_BOARD_MOVE;
		}
};

} // namespace GameEngine

#endif
