#ifndef STAGES_PLAYER_EQUIP_WEAPON_H
#define STAGES_PLAYER_EQUIP_WEAPON_H

#include <stdexcept>
#include <vector>

#include "game-engine/stages/common.h"
#include "game-engine/cards/common.h"
#include "game-engine/board.h"
#include "game-engine/stages/helper.h"

namespace GameEngine {

class StagePlayerEquipWeapon
{
	public:
		static const Stage stage = STAGE_PLAYER_EQUIP_WEAPON;

		static void Go(Board &board)
		{
			auto & player = board.player;
			const Board::PlayHandCardData &data = board.data.play_hand_card_data;

			auto playing_card = player.hand.GetCard(data.hand_card);
			player.hand.RemoveCard(data.hand_card);

#ifdef DEBUG
			if (playing_card.type != Card::TYPE_WEAPON) throw std::runtime_error("card type is not weapon");
#endif

			player.stat.crystal.CostCrystals(playing_card.cost);

			StageHelper::EquipWeapon(player, playing_card, data.target);

			board.stage = STAGE_PLAYER_CHOOSE_BOARD_MOVE;
		}
};

} // namespace GameEngine

#endif
