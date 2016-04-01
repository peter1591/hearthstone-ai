#ifndef STAGES_OPPONENT_PUT_MINION_H
#define STAGES_OPPONENT_PUT_MINION_H

#include <stdexcept>
#include <vector>

#include "game-engine/stages/common.h"
#include "game-engine/board.h"

namespace GameEngine {

class StageOpponentPutMinion
{
	public:
		static const Stage stage = STAGE_OPPONENT_PUT_MINION;

		static void Go(Board &board)
		{
			// TODO: implement
			throw std::runtime_error("not implemented");
		}
};

} // namespace GameEngine

#endif
