#ifndef GAME_ENGINE_MOVE_H
#define GAME_ENGINE_MOVE_H

#include <functional>
#include <string>
#include "card.h"
#include "hand.h"

namespace GameEngine {

class Move
{
	public:
		enum Action {
			ACTION_UNKNOWN,
			ACTION_GAME_FLOW,
			ACTION_PLAY_HAND_CARD_MINION,
			ACTION_OPPONENT_PLAY_MINION,
			ACTION_PLAYER_ATTACK,
			ACTION_OPPONENT_ATTACK,
			ACTION_END_TURN,
		};

		struct GameFlowData {
			unsigned int rand_seed;
		};

		struct PlayHandCardMinionData {
			Hand::Locator hand_card;
#ifdef CHOOSE_WHERE_TO_PUT_MINION
			int location; // where to put the minion
#endif
		};

		struct OpponentPlayMinionData {
			Card card;
#ifdef CHOOSE_WHERE_TO_PUT_MINION
			int location; // where to put the minion
#endif
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
		};

	public:
		Move() : action(ACTION_UNKNOWN) {}
		bool operator==(const Move &rhs) const;
		bool operator!=(const Move &rhs) const;

		std::string GetDebugString() const;

	public:
		static const Move & GetGameFlowMove(unsigned int rand_seed)
		{
			static Move game_flow_move;
			game_flow_move.action = ACTION_GAME_FLOW;
			game_flow_move.data.game_flow_data.rand_seed = rand_seed;
			return game_flow_move;
		}

	public:
		Action action;
		Data data;
};

inline bool Move::operator==(const Move &rhs) const
{
	if (this->action != rhs.action) return false;

	switch (this->action) {
	case ACTION_UNKNOWN:
		break;

	case ACTION_GAME_FLOW:
		if (this->data.game_flow_data.rand_seed != rhs.data.game_flow_data.rand_seed) return false;
		break;

	case ACTION_PLAY_HAND_CARD_MINION:
		if (this->data.play_hand_card_minion_data.hand_card != rhs.data.play_hand_card_minion_data.hand_card) return false;
#ifdef CHOOSE_WHERE_TO_PUT_MINION
		if (this->data.play_hand_card_minion_data.location != rhs.data.play_hand_card_minion_data.location) return false;
#endif
		break;

	case ACTION_OPPONENT_PLAY_MINION:
		if (this->data.opponent_play_minion_data.card != rhs.data.opponent_play_minion_data.card) return false;
#ifdef CHOOSE_WHERE_TO_PUT_MINION
		if (this->data.opponent_play_minion_data.location != rhs.data.opponent_play_minion_data.location) return false;
#endif
		break;

	case ACTION_PLAYER_ATTACK:
		if (this->data.player_attack_data.attacked_idx != rhs.data.player_attack_data.attacked_idx) return false;
		if (this->data.player_attack_data.attacker_idx != rhs.data.player_attack_data.attacker_idx) return false;
		break;

	case ACTION_OPPONENT_ATTACK:
		if (this->data.opponent_attack_data.attacked_idx != rhs.data.opponent_attack_data.attacked_idx) return false;
		if (this->data.opponent_attack_data.attacker_idx != rhs.data.opponent_attack_data.attacker_idx) return false;
		break;

	case ACTION_END_TURN:
		break;
	}

	return true;
}

inline bool Move::operator!=(const Move &rhs) const
{
	return !(*this == rhs);
}

} // namespace GameEngine

namespace std {
	template <> struct hash<GameEngine::Move> {
		typedef GameEngine::Move argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			GameEngine::hash_combine(result, hash<int>()(s.action));

			// hash for the union
			switch (s.action) {
			case GameEngine::Move::ACTION_UNKNOWN:
				break;

			case GameEngine::Move::ACTION_GAME_FLOW:
				GameEngine::hash_combine(result, hash<unsigned int>()(s.data.game_flow_data.rand_seed));
				break;

			case GameEngine::Move::ACTION_PLAY_HAND_CARD_MINION:
				GameEngine::hash_combine(result, hash<int>()(s.data.play_hand_card_minion_data.hand_card));
#ifdef CHOOSE_WHERE_TO_PUT_MINION
				GameEngine::hash_combine(result, hash<int>()(s.data.play_hand_card_minion_data.location));
#endif
				break;

			case GameEngine::Move::ACTION_OPPONENT_PLAY_MINION:
				GameEngine::hash_combine(result, hash<GameEngine::Card>()(s.data.opponent_play_minion_data.card));
#ifdef CHOOSE_WHERE_TO_PUT_MINION
				GameEngine::hash_combine(result, hash<int>()(s.data.opponent_play_minion_data.location));
#endif
				break;

			case GameEngine::Move::ACTION_PLAYER_ATTACK:
				GameEngine::hash_combine(result, hash<int>()(s.data.player_attack_data.attacked_idx));
				GameEngine::hash_combine(result, hash<int>()(s.data.player_attack_data.attacker_idx));
				break;

			case GameEngine::Move::ACTION_OPPONENT_ATTACK:
				GameEngine::hash_combine(result, hash<int>()(s.data.opponent_attack_data.attacked_idx));
				GameEngine::hash_combine(result, hash<int>()(s.data.opponent_attack_data.attacker_idx));
				break;

			case GameEngine::Move::ACTION_END_TURN:
				break;
			}

			return result;
		}
	};
}

#endif
