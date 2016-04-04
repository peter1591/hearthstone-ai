#ifndef STAGES_PLAYER_OPPONENT_SPELL_H
#define STAGES_PLAYER_OPPONENT_SPELL_H

#include <stdexcept>
#include <vector>

#include "game-engine/stages/common.h"
#include "game-engine/cards/common.h"
#include "game-engine/board.h"
#include "game-engine/stages/helper.h"

namespace GameEngine {

	class StageOpponentPlaySpell
	{
	public:
		static const Stage stage = STAGE_OPPONENT_PLAY_SPELL;

		static void Go(Board &board)
		{
			auto & player = board.opponent;
			const Board::PlayHandCardData &data = board.data.play_hand_card_data;

			auto playing_card = player.hand.GetCard(data.hand_card);
			player.hand.RemoveCard(data.hand_card);

#ifdef DEBUG
			if (playing_card.type != Card::TYPE_SPELL) throw std::runtime_error("card type is not spell");
#endif

			player.stat.crystal.CostCrystals(playing_card.cost);

			StageHelper::PlaySpell(player, playing_card, data.target);

			board.stage = STAGE_OPPONENT_CHOOSE_BOARD_MOVE;
		}
	};

} // namespace GameEngine

#endif
