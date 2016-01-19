#include <string>
#include <sstream>
#include <stdexcept>

#include "move.h"

std::string Move::GetDebugString() const
{
	std::ostringstream oss;

	switch (this->action)
	{
		case Move::ACTION_GAME_FLOW:
			oss << "[Game flow]";
			break;

		case Move::ACTION_PLAY_HAND_CARD_MINION:
			oss << "[Play hand card minion] hand idx = " << this->data.play_hand_card_minion_data.idx_hand_card
				<< ", put location = " << this->data.play_hand_card_minion_data.location;
			break;
		case Move::ACTION_ATTACK:
			oss << "[Attack] attacking = " << this->data.attack_data.attacking_idx
				<< ", attacked = " << this->data.attack_data.attacked_idx;
			break;
		case Move::ACTION_END_TURN:
			oss << "[End turn]";
			break;

		default:
			throw std::runtime_error("unknown action for Move::DebugPrint()");
			break;
	}

	return oss.str();
}
