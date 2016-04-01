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
			// TODO: implement
			throw std::runtime_error("not implemented");;
		}
};

} // namespace GameEngine

#endif
