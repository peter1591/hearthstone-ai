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

#ifdef DEBUG
			if (playing_card.type != Card::TYPE_SPELL) throw std::runtime_error("card type is not spell");
#endif

			player.stat.crystal.CostCrystals(playing_card.cost);

			if (StageHelper::PlaySpell(player, playing_card, data.target)) return; // game ends

			board.stage = STAGE_PLAYER_CHOOSE_BOARD_MOVE;
		}
	};

} // namespace GameEngine

#endif
