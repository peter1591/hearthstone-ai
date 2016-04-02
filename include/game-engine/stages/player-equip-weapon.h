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
			const Board::PlayHandWeaponData &data = board.data.play_hand_weapon_data;

			auto playing_card = board.player.hand.GetCard(data.hand_card);
			board.player.hand.RemoveCard(data.hand_card);

			board.player.stat.crystal.CostCrystals(playing_card.cost);

			StageHelper::EquipWeapon(board.player, playing_card, data.data);

			board.stage = STAGE_PLAYER_CHOOSE_BOARD_MOVE;
		}
};

} // namespace GameEngine

#endif
