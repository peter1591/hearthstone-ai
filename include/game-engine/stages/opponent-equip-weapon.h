#ifndef STAGES_OPPONENT_EQUIP_WEAPON_H
#define STAGES_OPPONENT_EQUIP_WEAPON_H

#include <stdexcept>
#include <vector>

#include "game-engine/stages/common.h"
#include "game-engine/board.h"

namespace GameEngine {

class StageOpponentEquipWeapon
{
	public:
		static const Stage stage = STAGE_OPPONENT_EQUIP_WEAPON;

		static void Go(Board &board)
		{
			const Board::OpponentEquipWeaponData &data = board.data.opponent_equip_weapon_data;

			board.opponent_cards.PlayCardFromHand(data.card);

			board.opponent_stat.crystal.CostCrystals(data.card.cost);

			StageHelper::EquipWeapon(board, data.card, SLOT_OPPONENT_SIDE, data.data);
			
			board.stage = STAGE_OPPONENT_CHOOSE_BOARD_MOVE;
		}
};

} // namespace GameEngine

#endif
