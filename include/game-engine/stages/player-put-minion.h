#ifndef STAGES_PLAYER_PUT_MINION_H
#define STAGES_PLAYER_PUT_MINION_H

#include <stdexcept>
#include <vector>

#include "game-engine/stages/common.h"
#include "game-engine/cards/common.h"
#include "game-engine/board.h"
#include "game-engine/stages/helper.h"

namespace GameEngine {

class StagePlayerPutMinion
{
	public:
		static const Stage stage = STAGE_PLAYER_PUT_MINION;

		static void Go(Board &board)
		{
			if (StageHelper::PlayMinion(board.player, board.data.play_hand_minion_data)) return;
			board.stage = STAGE_PLAYER_CHOOSE_BOARD_MOVE;
		}
};

} // namespace GameEngine

#endif
