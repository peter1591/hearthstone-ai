#ifndef STAGES_PLAYER_PUT_MINION_H
#define STAGES_PLAYER_PUT_MINION_H

#include <stdexcept>
#include <vector>

#include "game-engine/stages/common.h"
#include "game-engine/cards/common.h"
#include "game-engine/board.h"
#include "game-engine/targetor.h"
#include "game-engine/stages/helper.h"

namespace GameEngine {

class StagePlayerPutMinion
{
	public:
		static const Stage stage = STAGE_PLAYER_PUT_MINION;

		static void Go(Board &board)
		{
			const Board::PlayerPlayMinionData &data = board.data.player_play_minion_data;

			auto playing_card = board.player_hand.GetCard(data.hand_card);
			board.player_hand.RemoveCard(data.hand_card);

			board.player_stat.crystal.CostCrystals(playing_card.cost);

			StageHelper::PlayMinion(board, playing_card, data.data);

			board.stage = STAGE_PLAYER_CHOOSE_BOARD_MOVE;
		}
};

} // namespace GameEngine

#endif
