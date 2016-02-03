#include <string>
#include <sstream>
#include <stdexcept>

#include "game-engine/move.h"

namespace GameEngine {

std::string Move::GetDebugString() const
{
	std::ostringstream oss;

	switch (this->action)
	{
		case Move::ACTION_GAME_FLOW:
			oss << "[Game flow] random: " << this->data.game_flow_data.rand_seed;
			break;

		case Move::ACTION_PLAYER_PLAY_MINION:
			oss << "[Player play minion] hand idx = " << this->data.player_play_minion_data.hand_card;
#ifdef CHOOSE_WHERE_TO_PUT_MINION
			oss << ", put location = " << this->data.player_play_minion_data.location;
#endif
			oss << ", target = " << this->data.player_play_minion_data.required_target;
			break;

		case Move::ACTION_OPPONENT_PLAY_MINION:
			oss << "[Opponent play minion] card: " << this->data.opponent_play_minion_data.card.id;
#ifdef CHOOSE_WHERE_TO_PUT_MINION
			oss << ", put location = " << this->data.opponent_play_minion_data.location;
#endif
			break;

		case Move::ACTION_ATTACK:
			oss << "[Attack] attacking = " << this->data.attack_data.attacker_idx
				<< ", attacked = " << this->data.attack_data.attacked_idx;
			break;

		case Move::ACTION_END_TURN:
			oss << "[End turn]";
			break;

		case Move::ACTION_UNKNOWN:
			throw std::runtime_error("unknown action for Move::DebugPrint()");
			break;
	}

	return oss.str();
}

} // namespace GameEngine
