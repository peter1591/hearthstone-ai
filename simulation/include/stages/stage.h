#ifndef _STAGES_STAGE_H
#define _STAGES_STAGE_H

enum Stage {
	STAGE_UNKNOWN = 0,

	STAGE_PLAYER_TURN_START,
	STAGE_PLAYER_CHOOSE_BOARD_MOVE, // play card from hand, attack, or end turn
	STAGE_PLAYER_TURN_END,
	STAGE_OPPONENT_TURN_START,
	STAGE_OPPONENT_CHOOSE_BOARD_MOVE, // play card from hand, attack, or end turn
	STAGE_OPPONENT_TURN_END,

	STAGE_WIN,
	STAGE_LOSS
};
#endif
