#ifndef STAGES_OPPONENT_ATTACK_H
#define STAGES_OPPONENT_ATTACK_H

#include <stdexcept>
#include <vector>

#include "stages/common.h"
#include "game-engine/board.h"

namespace GameEngine {

class StageOpponentAttack
{
	public:
		static const Stage stage = STAGE_OPPONENT_ATTACK;

		static void Go(Board &board)
		{
			const Board::OpponentAttackData &data = board.data.opponent_attack_data;

			// TODO: Moidfy HandleAttack() to remove this
			int normalized_attacker_idx = data.attacker_idx - Targetor::GetOpponentMinionIndex(0);
			int normalized_attacked_idx = data.attacked_idx - Targetor::GetPlayerMinionIndex(0);

			StageHelper::HandleAttack(
					board.opponent_stat, board.opponent_minions.GetMinions(), normalized_attacker_idx,
					board.player_stat, board.player_minions.GetMinions(), normalized_attacked_idx);

			if (StageHelper::CheckWinLoss(board)) return; // game ends;

			board.stage = STAGE_OPPONENT_CHOOSE_BOARD_MOVE;
		}
};

} // namespace GameEngine

#endif
