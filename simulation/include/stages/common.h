#ifndef _STAGES_COMMON_H
#define _STAGES_COMMON_H

// enable consistency checks? (introduce performance drops)
#define ENABLE_DEBUG_CHECKS

#include <vector>
#include "board.h"

class StageCommonUtilities
{
	public:
		static void GetGameFlowMove(std::vector<Move> &next_moves)
		{
			Move move;
			move.action = Move::ACTION_GAME_FLOW;
			next_moves.push_back(move);
		}
};

#endif
