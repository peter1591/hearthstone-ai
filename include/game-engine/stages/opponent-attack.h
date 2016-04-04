#ifndef STAGES_OPPONENT_ATTACK_H
#define STAGES_OPPONENT_ATTACK_H

#include <stdexcept>
#include <vector>

#include "game-engine/stages/common.h"
#include "game-engine/board.h"

namespace GameEngine {

class StageOpponentAttack
{
	public:
		static const Stage stage = STAGE_OPPONENT_ATTACK;

		static void Go(Board &board)
		{
			const Board::AttackData &data = board.data.attack_data;

			StageHelper::HandleAttack(board, data.attacker_idx, data.attacked_idx);

			// TODO: check dead in HandleAttack()
			if (StageHelper::CheckHeroMinionDead(board)) return; // game ends;

			board.stage = STAGE_OPPONENT_CHOOSE_BOARD_MOVE;
		}
};

} // namespace GameEngine

#endif
