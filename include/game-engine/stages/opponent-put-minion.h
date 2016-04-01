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
			const Board::PlayHandMinionData &data = board.data.play_hand_minion_data;

			auto playing_card = board.opponent.hand.GetCard(data.hand_card);
			board.opponent.hand.RemoveCard(data.hand_card);

			board.opponent.stat.crystal.CostCrystals(playing_card.cost);

			StageHelper::PlayMinion(board, playing_card, SLOT_OPPONENT_SIDE, data.data);

			board.stage = STAGE_OPPONENT_CHOOSE_BOARD_MOVE;
		}
};

} // namespace GameEngine

#endif
