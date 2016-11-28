#ifndef STAGES_PLAYER_OPPONENT_SPELL_H
#define STAGES_PLAYER_OPPONENT_SPELL_H

#include <stdexcept>
#include <vector>

#include "game-engine/stages/common.h"
#include "game-engine/cards/common.h"
#include "game-engine/board.h"
#include "game-engine/stages/helper.h"

namespace GameEngine {

	class StageOpponentPlaySpell
	{
	public:
		static const Stage stage = STAGE_OPPONENT_PLAY_SPELL;

		static void Go(Board &board)
		{
			if (StageHelper::PlaySpell(board.opponent, board.data.play_hand_card_data)) return; // game ends;
			board.stage = STAGE_OPPONENT_CHOOSE_BOARD_MOVE;
		}
	};

} // namespace GameEngine

#endif
