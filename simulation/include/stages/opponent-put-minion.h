#ifndef STAGES_OPPONENT_PUT_MINION_H
#define STAGES_OPPONENT_PUT_MINION_H

#include <stdexcept>
#include <vector>

#include "stages/common.h"
#include "board.h"

class StageOpponentPutMinion
{
	public:
		static const Stage stage = STAGE_OPPONENT_PUT_MINION;
		static std::string GetStageStringName() { return "StageOpponentPutMinion"; }

		static void Go(Board &board)
		{
			Minion minion;
			const Board::OpponentPutMinionData &data = board.data.opponent_put_minion_data;

			board.opponent_stat.crystal.CostCrystals(data.card.cost);

			// TODO: handle battlecry
			minion.card_id = data.card.id;
			minion.max_hp = data.card.data.minion.hp;
			minion.hp = minion.max_hp;
			minion.attack = data.card.data.minion.attack;

#ifdef CHOOSE_WHERE_TO_PUT_MINION
			board.opponent_minions.AddMinion(minion, data.location);
#else
			board.opponent_minions.AddMinion(minion); // add to the rightmost
#endif

			board.opponent_cards.PlayCardFromHand(data.card);

			board.stage = STAGE_OPPONENT_CHOOSE_BOARD_MOVE;
		}
};

#endif
