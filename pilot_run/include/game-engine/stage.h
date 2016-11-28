#ifndef _STAGES_STAGE_H
#define _STAGES_STAGE_H

#include <functional>

namespace GameEngine {

enum StageType {
	STAGE_TYPE_FLAG = 0xF000,
	STAGE_TYPE_PLAYER = 0x0000,
	STAGE_TYPE_OPPONENT = 0x1000,
	STAGE_TYPE_GAME_FLOW = 0x2000, // game flow including randoms
	STAGE_TYPE_GAME_END = 0x3000
};

enum Stage {
	STAGE_UNKNOWN = 0,

	// player turns
	STAGE_PLAYER_CHOOSE_BOARD_MOVE = STAGE_TYPE_PLAYER + 1,

	// opponent turns
	STAGE_OPPONENT_CHOOSE_BOARD_MOVE = STAGE_TYPE_OPPONENT + 1,

	// game flow
	STAGE_PLAYER_TURN_START = STAGE_TYPE_GAME_FLOW + 1,
	STAGE_PLAYER_PUT_MINION,
	STAGE_PLAYER_EQUIP_WEAPON,
	STAGE_PLAYER_PLAY_SPELL,
	STAGE_PLAYER_ATTACK,
	STAGE_PLAYER_USE_HERO_POWER,
	STAGE_PLAYER_TURN_END,
	STAGE_OPPONENT_TURN_START,
	STAGE_OPPONENT_PUT_MINION,
	STAGE_OPPONENT_EQUIP_WEAPON,
	STAGE_OPPONENT_PLAY_SPELL,
	STAGE_OPPONENT_ATTACK,
	STAGE_OPPONENT_USE_HERO_POWER,
	STAGE_OPPONENT_TURN_END,

	// game ended
	STAGE_WIN = STAGE_TYPE_GAME_END + 1,
	STAGE_LOSS,
};

} // namespace GameEngine

namespace std {
	template <> struct hash<GameEngine::Stage> {
		typedef GameEngine::Stage argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			GameEngine::hash_combine(result, hash<int>()((int)s));

			return result;
		}
	};

}

#endif
