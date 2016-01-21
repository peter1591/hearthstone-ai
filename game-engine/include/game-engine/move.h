#ifndef GAME_ENGINE_MOVE_H
#define GAME_ENGINE_MOVE_H

#include <string>
#include "card.h"

namespace GameEngine {

class Move
{
	public:
		enum {
			ACTION_UNKNOWN,
			ACTION_GAME_FLOW,
			ACTION_PLAY_HAND_CARD_MINION,
			ACTION_OPPONENT_PLAY_MINION,
			ACTION_PLAYER_ATTACK,
			ACTION_OPPONENT_ATTACK,
			ACTION_END_TURN,
		} action;

		struct GameFlowData {
		};

		struct PlayHandCardMinionData {
			int idx_hand_card; // play the 'idx' hand card
			int location; // where to put the minion
		};

		struct OpponentPlayMinionData {
			Card card;
			int location; // where to put the minion
		};

		struct PlayerAttackData {
			int attacker_idx; // -1 indicates the hero
			int attacked_idx; // -1 indicates the hero
		};

		struct OpponentAttackData {
			int attacker_idx; // -1 indicates the hero
			int attacked_idx; // -1 indicates the hero
		};

		union Data {
			GameFlowData game_flow_data;
			PlayHandCardMinionData play_hand_card_minion_data;
			OpponentPlayMinionData opponent_play_minion_data;
			PlayerAttackData player_attack_data;
			OpponentAttackData opponent_attack_data;

			Data() {}
		} data;

		Move() : action(ACTION_UNKNOWN) {}

		std::string GetDebugString() const;

	public:
		static const Move & GetGameFlowMove()
		{
			static Move game_flow_move;
			game_flow_move.action = ACTION_GAME_FLOW;
			return game_flow_move;
		}
};

} // namespace GameEngine

#endif
