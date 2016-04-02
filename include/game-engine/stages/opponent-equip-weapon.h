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
			const Board::PlayHandWeaponData &data = board.data.play_hand_weapon_data;

			auto playing_card = board.opponent.hand.GetCard(data.hand_card);
			board.opponent.hand.RemoveCard(data.hand_card);

			board.opponent.stat.crystal.CostCrystals(playing_card.cost);

			StageHelper::EquipWeapon(board.opponent, playing_card, data.data);

			board.stage = STAGE_OPPONENT_CHOOSE_BOARD_MOVE;
		}
};

} // namespace GameEngine

#endif
