#ifndef GAME_ENGINE_MOVE_H
#define GAME_ENGINE_MOVE_H

#include <functional>
#include <string>
#include "card.h"
#include "hand.h"
#include "game-engine/slot-index.h"

namespace GameEngine {

class Move
{
	public:
		enum Action {
			ACTION_UNKNOWN,
			ACTION_GAME_FLOW,
			ACTION_PLAY_MINION,
			ACTION_PLAYER_EQUIP_WEAPON,
			ACTION_OPPONENT_EQUIP_WEAPON,
			ACTION_ATTACK,
			ACTION_END_TURN,
		};

		struct PlayMinionData
		{
			SlotIndex put_location;
			SlotIndex target;

			bool operator==(PlayMinionData const& rhs) const
			{
				if (this->put_location != rhs.put_location) return false;
				if (this->target != rhs.target) return false;
				return true;
			}

			bool operator!=(PlayMinionData const& rhs) const { return !(*this == rhs); }
		};

		struct EquipWeaponData
		{
			SlotIndex target;

			bool operator==(EquipWeaponData const& rhs) const
			{
				if (this->target != rhs.target) return false;
				return true;
			}

			bool operator!=(EquipWeaponData const& rhs) const { return !(*this == rhs); }
		};

		struct PlayHandMinionData {
			Hand::Locator hand_card;
			int card_id; // to distinguish moves for different boards merged into one MCTS tree node

			PlayMinionData data;

			bool operator==(PlayHandMinionData const& rhs) const
			{
				if (this->hand_card != rhs.hand_card) return false;
				if (this->data != rhs.data) return false;
				if (this->card_id != rhs.card_id) return false;
				return true;
			}

			bool operator!=(PlayHandMinionData const& rhs) const { return !(*this == rhs); }
		};

		struct PlayerEquipWeaponData {
			Hand::Locator hand_card;
			int card_id; // to distinguish moves for different boards merged into one MCTS tree node
			EquipWeaponData data;

			bool operator==(PlayerEquipWeaponData const& rhs) const
			{
				if (this->hand_card != rhs.hand_card) return false;
				if (this->data != rhs.data) return false;
				if (this->card_id != rhs.card_id) return false;
				return true;
			}

			bool operator!=(PlayerEquipWeaponData const& rhs) const { return !(*this == rhs); }
		};

		struct OpponentEquipWeaponData {
			Card card;
			EquipWeaponData data;

			bool operator==(OpponentEquipWeaponData const& rhs) const
			{
				if (this->card != rhs.card) return false;
				if (this->data != rhs.data) return false;
				return true;
			}

			bool operator!=(OpponentEquipWeaponData const& rhs) const { return !(*this == rhs); }
		};

		struct AttackData {
			SlotIndex attacker_idx;
			SlotIndex attacked_idx;

			bool operator==(AttackData const& rhs) const
			{
				if (this->attacker_idx != rhs.attacker_idx) return false;
				if (this->attacked_idx != rhs.attacked_idx) return false;
				return true;
			}

			bool operator!=(AttackData const& rhs) const { return !(*this == rhs); }
		};

		union Data {
			PlayHandMinionData play_hand_minion_data;
			PlayerEquipWeaponData player_equip_weapon_data;
			OpponentEquipWeaponData opponent_equip_weapon_data;
			AttackData attack_data;
		};

	public:
		Move() : action(ACTION_UNKNOWN) {}
		bool operator==(const Move &rhs) const;
		bool operator!=(const Move &rhs) const;

		std::string GetDebugString() const;

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
		break;

	case ACTION_PLAY_MINION:
		if (this->data.play_hand_minion_data != rhs.data.play_hand_minion_data) return false;
		break;

	case ACTION_PLAYER_EQUIP_WEAPON:
		if (this->data.player_equip_weapon_data != rhs.data.player_equip_weapon_data) return false;
		break;

	case ACTION_OPPONENT_EQUIP_WEAPON:
		if (this->data.opponent_equip_weapon_data != rhs.data.opponent_equip_weapon_data) return false;
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

inline std::string Move::GetDebugString() const
{
	std::ostringstream oss;

	switch (this->action)
	{
	case Move::ACTION_GAME_FLOW:
		oss << "[Game flow]";
		break;

	case Move::ACTION_PLAY_MINION:
		oss << "[Play minion] hand idx = " << this->data.play_hand_minion_data.hand_card;
		oss << ", card id = " << this->data.play_hand_minion_data.card_id;
		oss << ", put location = " << this->data.play_hand_minion_data.data.put_location;
		oss << ", target = " << this->data.play_hand_minion_data.data.target;
		break;

	case Move::ACTION_PLAYER_EQUIP_WEAPON:
		oss << "[Player equip weapon] hand idx = " << this->data.player_equip_weapon_data.hand_card;
		oss << ", card id = " << this->data.player_equip_weapon_data.card_id;
		oss << ", target = " << this->data.player_equip_weapon_data.data.target;
		break;

	case Move::ACTION_OPPONENT_EQUIP_WEAPON:
		oss << "[Opponent equip weapon] card = " << this->data.opponent_equip_weapon_data.card.id;
		oss << ", target = " << this->data.opponent_equip_weapon_data.data.target;
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

namespace std {
	template <> struct hash<GameEngine::Move::PlayMinionData> {
		typedef GameEngine::Move::PlayMinionData argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			GameEngine::hash_combine(result, s.put_location);
			GameEngine::hash_combine(result, s.target);

			return result;
		}
	};

	template <> struct hash<GameEngine::Move::EquipWeaponData> {
		typedef GameEngine::Move::EquipWeaponData argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			GameEngine::hash_combine(result, s.target);

			return result;
		}
	};

	template <> struct hash<GameEngine::Move::PlayHandMinionData> {
		typedef GameEngine::Move::PlayHandMinionData argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			GameEngine::hash_combine(result, s.hand_card);
			GameEngine::hash_combine(result, s.card_id);
			GameEngine::hash_combine(result, s.data);

			return result;
		}
	};

	template <> struct hash<GameEngine::Move::PlayerEquipWeaponData> {
		typedef GameEngine::Move::PlayerEquipWeaponData argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			GameEngine::hash_combine(result, s.hand_card);
			GameEngine::hash_combine(result, s.data);
			GameEngine::hash_combine(result, s.card_id);

			return result;
		}
	};

	template <> struct hash<GameEngine::Move::OpponentEquipWeaponData> {
		typedef GameEngine::Move::OpponentEquipWeaponData argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			GameEngine::hash_combine(result, s.card);
			GameEngine::hash_combine(result, s.data);

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
				break;

			case GameEngine::Move::ACTION_PLAY_MINION:
				GameEngine::hash_combine(result, s.data.play_hand_minion_data);
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
