#ifndef STAGES_PLAYER_PUT_MINION_H
#define STAGES_PLAYER_PUT_MINION_H

#include <stdexcept>
#include <vector>

#include "game-engine/stages/common.h"
#include "game-engine/cards/common.h"
#include "game-engine/board.h"
#include "game-engine/targetor.h"

namespace GameEngine {

class StagePlayerPutMinion
{
	public:
		static const Stage stage = STAGE_PLAYER_PUT_MINION;

		static void Go(Board &board)
		{
			Minion minion;
			const Board::PlayerPutMinionData &data = board.data.player_put_minion_data;

			auto playing_card = board.player_hand.GetCard(data.hand_card);

			board.player_hand.RemoveCard(data.hand_card);

			board.player_stat.crystal.CostCrystals(playing_card.cost);

			Cards::CardCallbackManager::BattleCry(playing_card.id, board);

			minion.Summon(playing_card);

#ifdef CHOOSE_WHERE_TO_PUT_MINION
			int put_minion_location = data.location - Targetor::GetPlayerMinionIndex(0);
			board.player_minions.AddMinion(minion, put_minion_location);
#else
			board.player_minions.AddMinion(minion); // add to the rightmost
#endif

			board.stage = STAGE_PLAYER_CHOOSE_BOARD_MOVE;
		}
};

} // namespace GameEngine

#endif
