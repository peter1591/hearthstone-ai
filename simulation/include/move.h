#ifndef MOVE_H
#define MOVE_H

#include <string>

class Move
{
	public:
		enum {
			ACTION_UNKNOWN,
			ACTION_GAME_FLOW,
			ACTION_PLAY_HAND_CARD_MINION,
			ACTION_ATTACK,
			ACTION_END_TURN,
		} action;

		struct GameFlowData {
		};

		struct PlayHandCardMinionData {
			int idx_hand_card; // play the 'idx' hand card
			int location; // where to put the minion
		};

		struct AttackData {
			int attacking_idx; // 0 indicates the hero
			int attacked_idx; // 0 indicates the hero
		};

		union Data {
			GameFlowData game_flow_data;
			PlayHandCardMinionData play_hand_card_minion_data;
			AttackData attack_data;

			Data() {}
		} data;

		Move() : action(ACTION_UNKNOWN) {}

		std::string GetDebugString() const;
};

#endif
