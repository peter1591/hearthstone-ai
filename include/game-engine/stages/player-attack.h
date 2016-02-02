#ifndef STAGES_PLAYER_ATTACK_H
#define STAGES_PLAYER_ATTACK_H

#include <stdexcept>
#include <vector>

#include "game-engine/stages/common.h"
#include "game-engine/board.h"
#include "game-engine/targetor.h"

namespace GameEngine {

class StagePlayerAttack
{
	public:
		static const Stage stage = STAGE_PLAYER_ATTACK;

		static void Go(Board &board)
		{
			const Board::PlayerAttackData &data = board.data.player_attack_data;

			StageHelper::HandleAttack(board, data.attacker_idx, data.attacked_idx);

			if (StageHelper::CheckWinLoss(board)) return; // game ends;

			board.stage = STAGE_PLAYER_CHOOSE_BOARD_MOVE;
		}
};

} // namespace GameEngine

#endif
