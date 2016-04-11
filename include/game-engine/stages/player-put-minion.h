#ifndef STAGES_PLAYER_PUT_MINION_H
#define STAGES_PLAYER_PUT_MINION_H

#include <stdexcept>
#include <vector>

#include "game-engine/stages/common.h"
#include "game-engine/cards/common.h"
#include "game-engine/board.h"
#include "game-engine/stages/helper.h"

namespace GameEngine {

class StagePlayerPutMinion
{
	public:
		static const Stage stage = STAGE_PLAYER_PUT_MINION;

		static void Go(Board &board)
		{
			auto & player = board.player;
			Board::PlayHandMinionData &data = board.data.play_hand_minion_data;

			auto playing_card = player.hand.GetCard(data.hand_card);
			player.hand.RemoveCard(data.hand_card);

#ifdef DEBUG
			if (playing_card.type != Card::TYPE_MINION) throw std::runtime_error("card type is not minion");
#endif

			player.stat.crystal.CostCrystals(playing_card.cost);

#ifndef CHOOSE_WHERE_TO_PUT_MINION
			if (data.data.put_location != SLOT_PLAYER_MINION_START) throw std::runtime_error("check failed");
			data.data.put_location = (SlotIndex)(SLOT_PLAYER_MINION_START + board.random_generator.GetRandom(player.minions.GetMinionCount() + 1));
#endif
			if (StageHelper::PlayMinion(player, playing_card, data.data)) return;

			board.stage = STAGE_PLAYER_CHOOSE_BOARD_MOVE;
		}
};

} // namespace GameEngine

#endif
