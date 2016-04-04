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
			auto & player = board.player;
			const Board::PlayHandCardData &data = board.data.play_hand_card_data;

			auto playing_card = player.hand.GetCard(data.hand_card);
			player.hand.RemoveCard(data.hand_card);

			player.stat.crystal.CostCrystals(playing_card.cost);

			StageHelper::PlaySpell(player, playing_card, data.data);

			board.stage = STAGE_PLAYER_CHOOSE_BOARD_MOVE;
		}
	};

} // namespace GameEngine

#endif
