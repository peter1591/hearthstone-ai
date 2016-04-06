#pragma once

#include <stdexcept>
#include <vector>

#include "game-engine/stages/common.h"
#include "game-engine/cards/common.h"
#include "game-engine/board.h"
#include "game-engine/stages/helper.h"

namespace GameEngine {

	class StageOpponentUseHeroPower
	{
	public:
		static const Stage stage = STAGE_OPPONENT_USE_HERO_POWER;

		static void Go(Board &board)
		{
			const Board::UseHeroPowerData &data = board.data.use_hero_power_data;
			if (StageHelper::UseHeroPower(board.opponent, data.target)) return; // game ends

			board.stage = STAGE_OPPONENT_CHOOSE_BOARD_MOVE;
		}
	};

} // namespace GameEngine