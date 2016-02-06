#ifndef STAGES_PLAYER_ATTACK_H
#define STAGES_PLAYER_ATTACK_H

#include <stdexcept>
#include <vector>

#include "game-engine/stages/common.h"
#include "game-engine/board.h"

namespace GameEngine {

class StagePlayerAttack
{
	public:
		static const Stage stage = STAGE_PLAYER_ATTACK;

		static void Go(Board &board)
		{
			const Board::AttackData &data = board.data.attack_data;

			StageHelper::HandleAttack(board, data.attacker_idx, data.attacked_idx);

			if (StageHelper::CheckHeroMinionDead(board)) return; // game ends;

			board.stage = STAGE_PLAYER_CHOOSE_BOARD_MOVE;
		}
};

} // namespace GameEngine

#endif
