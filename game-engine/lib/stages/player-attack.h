#ifndef STAGES_PLAYER_ATTACK_H
#define STAGES_PLAYER_ATTACK_H

#include <stdexcept>
#include <vector>

#include "stages/common.h"
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

			// TODO
			int normalized_attacker_idx = data.attacker_idx - Targetor::GetPlayerMinionIndex(0);
			int normalized_attacked_idx = data.attacked_idx - Targetor::GetOpponentMinionIndex(0);

			StageHelper::HandleAttack(
					board.player_stat, board.player_minions.GetMinions(), normalized_attacker_idx,
					board.opponent_stat, board.opponent_minions.GetMinions(), normalized_attacked_idx);

			if (StageHelper::CheckWinLoss(board)) return; // game ends;

			board.stage = STAGE_PLAYER_CHOOSE_BOARD_MOVE;
		}
};

} // namespace GameEngine

#endif
