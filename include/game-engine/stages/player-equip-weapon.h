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
			if (StageHelper::EquipWeapon(board.player, board.data.play_hand_card_data)) return; // game ends
			board.stage = STAGE_PLAYER_CHOOSE_BOARD_MOVE;
		}
};

} // namespace GameEngine

#endif
