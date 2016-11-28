#ifndef STAGES_PLAYER_PLAY_SPELL_H
#define STAGES_PLAYER_PLAY_SPELL_H

#include <stdexcept>
#include <vector>

#include "game-engine/stages/common.h"
#include "game-engine/cards/common.h"
#include "game-engine/board.h"
#include "game-engine/stages/helper.h"

namespace GameEngine {

	class StagePlayerPlaySpell
	{
	public:
		static const Stage stage = STAGE_PLAYER_PLAY_SPELL;

		static void Go(Board &board)
		{
			if (StageHelper::PlaySpell(board.player, board.data.play_hand_card_data)) return; // game ends;
			board.stage = STAGE_PLAYER_CHOOSE_BOARD_MOVE;
		}
	};

} // namespace GameEngine

#endif
