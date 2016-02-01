#ifndef STAGES_OPPONENT_PUT_MINION_H
#define STAGES_OPPONENT_PUT_MINION_H

#include <stdexcept>
#include <vector>

#include "stages/common.h"
#include "game-engine/board.h"

namespace GameEngine {

class StageOpponentPutMinion
{
	public:
		static const Stage stage = STAGE_OPPONENT_PUT_MINION;

		static void Go(Board &board)
		{
			Minion minion;
			const Board::OpponentPutMinionData &data = board.data.opponent_put_minion_data;

			board.opponent_cards.PlayCardFromHand(data.card);

			board.opponent_stat.crystal.CostCrystals(data.card.cost);

			// Get initialized minion with original card's attack/hp/max-hp
			minion.Summon(data.card);

			// TODO: handle battlecry


#ifdef CHOOSE_WHERE_TO_PUT_MINION
			board.opponent_minions.AddMinion(minion, data.location);
#else
			board.opponent_minions.AddMinion(minion); // add to the rightmost
#endif

			board.stage = STAGE_OPPONENT_CHOOSE_BOARD_MOVE;
		}
};

} // namespace GameEngine

#endif
