#ifndef STAGES_PLAYER_ATTACK_H
#define STAGES_PLAYER_ATTACK_H

#include <stdexcept>
#include <vector>

#include "stages/common.h"
#include "board.h"

class StagePlayerAttack
{
	public:
		static const Stage stage = STAGE_PLAYER_ATTACK;

		static void Go(Board &board)
		{
			const Board::PlayerAttackData &data = board.data.player_attack_data;

			StageHelper::HandleAttack(
					board.player_stat, board.player_minions.GetMinions(), data.attacker_idx,
					board.opponent_stat, board.opponent_minions.GetMinions(), data.attacked_idx);

			if (StageHelper::CheckWinLoss(board)) return; // game ends;

			board.stage = STAGE_PLAYER_CHOOSE_BOARD_MOVE;
		}
};

#endif
