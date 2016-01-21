#ifndef STAGES_PLAYER_PUT_MINION_H
#define STAGES_PLAYER_PUT_MINION_H

#include <stdexcept>
#include <vector>

#include "stages/common.h"
#include "board.h"

namespace GameEngine {

class StagePlayerPutMinion
{
	public:
		static const Stage stage = STAGE_PLAYER_PUT_MINION;

		static void Go(Board &board)
		{
			Minion minion;
			const Board::PlayerPutMinionData &data = board.data.player_put_minion_data;

			std::vector<Card>::const_iterator it_hand_card = board.player_hand.GetCards().begin() + data.idx_hand_card;

			board.player_stat.crystal.CostCrystals(it_hand_card->cost);

			// TODO: handle battlecry
			minion.Summon(*it_hand_card);

#ifdef CHOOSE_WHERE_TO_PUT_MINION
			board.player_minions.AddMinion(minion, data.location);
#else
			board.player_minions.AddMinion(minion); // add to the rightmost
#endif

			board.player_hand.RemoveCard(data.idx_hand_card);

			board.stage = STAGE_PLAYER_CHOOSE_BOARD_MOVE;
		}
};

} // namespace GameEngine

#endif
