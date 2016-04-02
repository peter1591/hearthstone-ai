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
			auto & player = board.opponent;
			const Board::PlayHandMinionData &data = board.data.play_hand_minion_data;

			auto playing_card = player.hand.GetCard(data.hand_card);
			player.hand.RemoveCard(data.hand_card);

			player.stat.crystal.CostCrystals(playing_card.cost);

			StageHelper::PlayMinion(player, playing_card, data.data);

			board.stage = STAGE_OPPONENT_CHOOSE_BOARD_MOVE;
		}
};

} // namespace GameEngine

#endif
