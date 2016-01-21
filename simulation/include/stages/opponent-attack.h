#ifndef STAGES_OPPONENT_ATTACK_H
#define STAGES_OPPONENT_ATTACK_H

#include <stdexcept>
#include <vector>

#include "stages/common.h"
#include "board.h"

class StageOpponentAttack
{
	public:
		static const Stage stage = STAGE_OPPONENT_ATTACK;

		static void Go(Board &board)
		{
			const Board::OpponentAttackData &data = board.data.opponent_attack_data;

			StageHelper::HandleAttack(
					board.opponent_stat, board.opponent_minions.GetMinions(), data.attacker_idx,
					board.player_stat, board.player_minions.GetMinions(), data.attacked_idx);

			if (StageHelper::CheckWinLoss(board)) return; // game ends;

			board.stage = STAGE_OPPONENT_CHOOSE_BOARD_MOVE;
		}
};

#endif
