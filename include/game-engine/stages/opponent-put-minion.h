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
			const Board::OpponentPlayMinionData &data = board.data.opponent_play_minion_data;

			board.opponent_cards.PlayCardFromHand(data.card);

			board.opponent_stat.crystal.CostCrystals(data.card.cost);

			StageHelper::PlayMinion(board, data.card, SLOT_OPPONENT_SIDE, data.data);
			
			board.stage = STAGE_OPPONENT_CHOOSE_BOARD_MOVE;
		}
};

} // namespace GameEngine

#endif
