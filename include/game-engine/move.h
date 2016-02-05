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
			ACTION_PLAYER_PLAY_MINION,
			ACTION_OPPONENT_PLAY_MINION,
			ACTION_ATTACK,
			ACTION_END_TURN,
		};

		struct GameFlowData {
			unsigned int rand_seed;
		};

		struct PlayerPlayMinionData {
			Hand::Locator hand_card;
#ifdef CHOOSE_WHERE_TO_PUT_MINION
			int location; // where to put the minion
#endif
			int required_target;

			bool operator==(PlayerPlayMinionData const& rhs) const
			{
				if (this->hand_card != rhs.hand_card) return false;
#ifdef CHOOSE_WHERE_TO_PUT_MINION
				if (this->location != rhs.location) return false;
#endif
				if (this->required_target != rhs.required_target) return false;
				return true;
			}

			bool operator!=(PlayerPlayMinionData const& rhs) const { return !(*this == rhs); }
		};

		struct OpponentPlayMinionData {
			Card card;
#ifdef CHOOSE_WHERE_TO_PUT_MINION
			int location; // where to put the minion
#endif

			bool operator==(OpponentPlayMinionData const& rhs) const
			{
				if (this->card != rhs.card) return false;
#ifdef CHOOSE_WHERE_TO_PUT_MINION
				if (this->location != rhs.location) return false;
#endif
				return true;
			}

			bool operator!=(OpponentPlayMinionData const& rhs) const { return !(*this == rhs); }
		};

		struct AttackData {
			int attacker_idx;
			int attacked_idx;

			bool operator==(AttackData const& rhs) const
			{
				if (this->attacker_idx != rhs.attacker_idx) return false;
				if (this->attacked_idx != rhs.attacked_idx) return false;
				return true;
			}

			bool operator!=(AttackData const& rhs) const { return !(*this == rhs); }
		};

		union Data {
			GameFlowData game_flow_data;
			PlayerPlayMinionData player_play_minion_data;
			OpponentPlayMinionData opponent_play_minion_data;
			AttackData attack_data;

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

	case ACTION_PLAYER_PLAY_MINION:
		if (this->data.player_play_minion_data != rhs.data.player_play_minion_data) return false;
		break;

	case ACTION_OPPONENT_PLAY_MINION:
		if (this->data.opponent_play_minion_data != rhs.data.opponent_play_minion_data) return false;
		break;

	case ACTION_ATTACK:
		if (this->data.attack_data != rhs.data.attack_data) return false;
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
	template <> struct hash<GameEngine::Move::PlayerPlayMinionData> {
		typedef GameEngine::Move::PlayerPlayMinionData argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			GameEngine::hash_combine(result, s.hand_card);
#ifdef CHOOSE_WHERE_TO_PUT_MINION
			GameEngine::hash_combine(result, s.location);
#endif
			GameEngine::hash_combine(result, s.required_target);

			return result;
		}
	};

	template <> struct hash<GameEngine::Move::OpponentPlayMinionData> {
		typedef GameEngine::Move::OpponentPlayMinionData argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			GameEngine::hash_combine(result, s.card);
#ifdef CHOOSE_WHERE_TO_PUT_MINION
			GameEngine::hash_combine(result, s.location);
#endif

			return result;
		}
	};

	template <> struct hash<GameEngine::Move::AttackData> {
		typedef GameEngine::Move::AttackData argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			GameEngine::hash_combine(result, s.attacker_idx);
			GameEngine::hash_combine(result, s.attacked_idx);

			return result;
		}
	};

	template <> struct hash<GameEngine::Move> {
		typedef GameEngine::Move argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			GameEngine::hash_combine(result, (int)s.action);

			// hash for the union
			switch (s.action) {
			case GameEngine::Move::ACTION_UNKNOWN:
				break;

			case GameEngine::Move::ACTION_GAME_FLOW:
				GameEngine::hash_combine(result, s.data.game_flow_data.rand_seed);
				break;

			case GameEngine::Move::ACTION_PLAYER_PLAY_MINION:
				GameEngine::hash_combine(result, s.data.player_play_minion_data);
				break;

			case GameEngine::Move::ACTION_OPPONENT_PLAY_MINION:
				GameEngine::hash_combine(result, s.data.opponent_play_minion_data);
				break;

			case GameEngine::Move::ACTION_ATTACK:
				GameEngine::hash_combine(result, s.data.attack_data);
				break;

			case GameEngine::Move::ACTION_END_TURN:
				break;
			}

			return result;
		}
	};
}

#endif
