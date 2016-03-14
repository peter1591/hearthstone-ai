#pragma once

#include <vector>
#include <list>
#include "move.h"
#include "slot-index.h"

namespace GameEngine {

class NextMoveGetter
{
public:
	class ItemPlayerPlayMinion
	{
	public:
		ItemPlayerPlayMinion(Hand::Locator hand_card, SlotIndex put_location, SlotIndexBitmap required_targets);
		ItemPlayerPlayMinion* Clone() const;
		bool GetNextMove(Move & move);
		bool operator==(ItemPlayerPlayMinion const& rhs) const;
		bool operator!=(ItemPlayerPlayMinion const& rhs) const;

	private:
		Hand::Locator hand_card;
		SlotIndex put_location;
		SlotIndexBitmap required_targets;
		bool done;
	};

	class ItemOpponentPlayMinion
	{
	public:
		ItemOpponentPlayMinion(Card card, SlotIndex put_location, SlotIndexBitmap required_targets);
		ItemOpponentPlayMinion* Clone() const;
		bool GetNextMove(Move & move);
		bool operator==(ItemOpponentPlayMinion const& rhs) const;
		bool operator!=(ItemOpponentPlayMinion const& rhs) const;

	private:
		Card playing_card;
		SlotIndex put_location;
		SlotIndexBitmap required_targets;
		bool done;
	};

	class ItemPlayerEquipWeapon
	{
	public:
		ItemPlayerEquipWeapon(Hand::Locator hand_card, SlotIndexBitmap required_targets);
		ItemPlayerEquipWeapon* Clone() const;
		bool GetNextMove(Move & move);
		bool operator==(ItemPlayerEquipWeapon const& rhs) const;
		bool operator!=(ItemPlayerEquipWeapon const& rhs) const;

	private:
		Hand::Locator hand_card;
		SlotIndexBitmap required_targets;
		bool done;
	};

	class ItemOpponentEquipWeapon
	{
	public:
		ItemOpponentEquipWeapon(Card card, SlotIndexBitmap required_targets);
		ItemOpponentEquipWeapon* Clone() const;
		bool GetNextMove(Move & move);
		bool operator==(ItemOpponentEquipWeapon const& rhs) const;
		bool operator!=(ItemOpponentEquipWeapon const& rhs) const;

	private:
		Card playing_card;
		SlotIndexBitmap required_targets;
		bool done;
	};

	class ItemAttack
	{
	public:
		ItemAttack(SlotIndexBitmap && attacker, SlotIndexBitmap && attacked);
		ItemAttack* Clone() const;
		bool GetNextMove(Move & move);
		bool operator==(ItemAttack const& rhs) const;
		bool operator!=(ItemAttack const& rhs) const;
	
	private:
		SlotIndexBitmap attacker;
		SlotIndexBitmap attacked;
		SlotIndexBitmap attacked_origin;
	};

public:
	void AddItem(Move const& move);
	void AddItem(Move && move);
	void AddItem(ItemAttack && items);
	void AddItem(ItemPlayerPlayMinion && items);
	void AddItem(ItemOpponentPlayMinion && items);
	void AddItem(ItemPlayerEquipWeapon && items);
	void AddItem(ItemOpponentEquipWeapon && items);
	void AddItems(std::list<Move> && items);
	void AddItems(std::list<ItemAttack> && items);
	void AddItems(std::list<ItemPlayerPlayMinion> && items);
	void AddItems(std::list<ItemOpponentPlayMinion> && items);
	void AddItems(std::list<ItemPlayerEquipWeapon> && items);
	void AddItems(std::list<ItemOpponentEquipWeapon> && items);

	bool GetNextMove(Move &move);
	bool Empty();

	bool operator==(NextMoveGetter const& rhs) const;
	bool operator!=(NextMoveGetter const& rhs) const;

private:
	template <typename T> bool GetNextMoveFromContainer(std::list<T> & container, Move &move);
	template <typename T> static bool IsEqual(std::list<T> const& lhs, std::list<T> const& rhs);

private:
	std::list<Move> moves;
	std::list<ItemAttack> items_player_attack;
	std::list<ItemPlayerPlayMinion> items_player_play_minion;
	std::list<ItemOpponentPlayMinion> items_opponent_play_minion;
	std::list<ItemPlayerEquipWeapon> items_player_equip_weapon;
	std::list<ItemOpponentEquipWeapon> items_opponent_equip_weapon;
};

} // namespace GameEngine

template<typename T>
inline bool GameEngine::NextMoveGetter::IsEqual(std::list<T> const & lhs, std::list<T> const & rhs)
{
	auto it_lhs = lhs.cbegin();
	auto it_rhs = rhs.cbegin();

	while (true)
	{
		if (it_lhs == lhs.cend()) break;
		if (it_rhs == rhs.cend()) break;

		if (*it_lhs != *it_rhs) return false;

		++it_lhs;
		++it_rhs;
	}

	if (it_lhs == lhs.cend() && it_rhs == rhs.cend()) return true;
	return false;
}

inline void GameEngine::NextMoveGetter::AddItem(GameEngine::Move const& item)
{
	this->moves.push_back(item);
}

inline void GameEngine::NextMoveGetter::AddItem(GameEngine::Move && item)
{
	this->moves.push_back(std::move(item));
}

inline void GameEngine::NextMoveGetter::AddItems(std::list<GameEngine::Move> && items)
{
	this->moves.splice(this->moves.end(), std::move(items));
}

inline void GameEngine::NextMoveGetter::AddItem(GameEngine::NextMoveGetter::ItemAttack && item)
{
	this->items_player_attack.push_back(std::move(item));
}

inline void GameEngine::NextMoveGetter::AddItems(std::list<GameEngine::NextMoveGetter::ItemAttack> && items)
{
	this->items_player_attack.splice(this->items_player_attack.end(), std::move(items));
}

inline void GameEngine::NextMoveGetter::AddItem(GameEngine::NextMoveGetter::ItemPlayerPlayMinion && item)
{
	this->items_player_play_minion.push_back(std::move(item));
}

inline void GameEngine::NextMoveGetter::AddItems(std::list<GameEngine::NextMoveGetter::ItemPlayerPlayMinion> && items)
{
	this->items_player_play_minion.splice(this->items_player_play_minion.end(), std::move(items));
}

inline void GameEngine::NextMoveGetter::AddItem(GameEngine::NextMoveGetter::ItemOpponentPlayMinion && item)
{
	this->items_opponent_play_minion.push_back(std::move(item));
}

inline void GameEngine::NextMoveGetter::AddItems(std::list<GameEngine::NextMoveGetter::ItemOpponentPlayMinion> && items)
{
	this->items_opponent_play_minion.splice(this->items_opponent_play_minion.end(), std::move(items));
}

inline void GameEngine::NextMoveGetter::AddItem(GameEngine::NextMoveGetter::ItemPlayerEquipWeapon && item)
{
	this->items_player_equip_weapon.push_back(std::move(item));
}

inline void GameEngine::NextMoveGetter::AddItems(std::list<GameEngine::NextMoveGetter::ItemPlayerEquipWeapon> && items)
{
	this->items_player_equip_weapon.splice(this->items_player_equip_weapon.end(), std::move(items));
}

inline void GameEngine::NextMoveGetter::AddItem(GameEngine::NextMoveGetter::ItemOpponentEquipWeapon && item)
{
	this->items_opponent_equip_weapon.push_back(std::move(item));
}

inline void GameEngine::NextMoveGetter::AddItems(std::list<GameEngine::NextMoveGetter::ItemOpponentEquipWeapon> && items)
{
	this->items_opponent_equip_weapon.splice(this->items_opponent_equip_weapon.end(), std::move(items));
}

template<typename T>
inline bool GameEngine::NextMoveGetter::GetNextMoveFromContainer(std::list<T>& container, Move &move)
{
	while (!container.empty())
	{
		T & item = container.back();
		if (item.GetNextMove(move)) return true;
		container.pop_back();
	}
	return false;
}

inline bool GameEngine::NextMoveGetter::GetNextMove(Move & move)
{
	if (!this->moves.empty()) {
		move = this->moves.back();
		this->moves.pop_back();
		return true;
	}

	if (this->GetNextMoveFromContainer(this->items_player_attack, move)) return true;
	if (this->GetNextMoveFromContainer(this->items_player_play_minion, move)) return true;
	if (this->GetNextMoveFromContainer(this->items_opponent_play_minion, move)) return true;
	if (this->GetNextMoveFromContainer(this->items_player_equip_weapon, move)) return true;
	if (this->GetNextMoveFromContainer(this->items_opponent_equip_weapon, move)) return true;
	return false;
}

inline bool GameEngine::NextMoveGetter::operator==(NextMoveGetter const & rhs) const
{
	if (!IsEqual(this->moves, rhs.moves)) return false;
	if (!IsEqual(this->items_player_attack, rhs.items_player_attack)) return false;
	if (!IsEqual(this->items_player_play_minion, rhs.items_player_play_minion)) return false;
	if (!IsEqual(this->items_opponent_play_minion, rhs.items_opponent_play_minion)) return false;
	if (!IsEqual(this->items_player_equip_weapon, rhs.items_player_equip_weapon)) return false;
	if (!IsEqual(this->items_opponent_equip_weapon, rhs.items_opponent_equip_weapon)) return false;
	return true;
}

inline bool GameEngine::NextMoveGetter::operator!=(NextMoveGetter const & rhs) const
{
	return !(*this == rhs);
}

inline GameEngine::NextMoveGetter::ItemPlayerPlayMinion::ItemPlayerPlayMinion(
	Hand::Locator hand_card, SlotIndex put_location, SlotIndexBitmap required_targets)
	: hand_card(hand_card), put_location(put_location), required_targets(required_targets)
{
	this->done = false;
}

inline GameEngine::NextMoveGetter::ItemPlayerPlayMinion * GameEngine::NextMoveGetter::ItemPlayerPlayMinion::Clone() const
{
	return new ItemPlayerPlayMinion(*this);
}

inline bool GameEngine::NextMoveGetter::ItemPlayerPlayMinion::GetNextMove(Move & move)
{
	if (this->done) return false;

	move.action = Move::ACTION_PLAYER_PLAY_MINION;
	move.data.player_play_minion_data.hand_card = this->hand_card;
	move.data.player_play_minion_data.data.put_location = this->put_location;

	if (this->required_targets.None())
	{
		move.data.player_play_minion_data.data.target = SLOT_INVALID;
		this->done = true;
	}
	else {
		move.data.player_play_minion_data.data.target = this->required_targets.GetOneTarget();
		this->required_targets.ClearOneTarget(move.data.player_play_minion_data.data.target);
		if (this->required_targets.None()) this->done = true;
	}
	return true;
}

inline bool GameEngine::NextMoveGetter::ItemPlayerPlayMinion::operator==(ItemPlayerPlayMinion const & rhs) const
{
	if (this->hand_card != rhs.hand_card) return false;
	if (this->put_location != rhs.put_location) return false;
	if (this->required_targets != rhs.required_targets) return false;
	return true;
}

inline bool GameEngine::NextMoveGetter::ItemPlayerPlayMinion::operator!=(ItemPlayerPlayMinion const & rhs) const
{
	return !(*this == rhs);
}

inline GameEngine::NextMoveGetter::ItemOpponentPlayMinion::ItemOpponentPlayMinion(
	Card playing_card, SlotIndex put_location, SlotIndexBitmap required_targets)
	: playing_card(playing_card), put_location(put_location), required_targets(required_targets)
{
	this->done = false;
}

inline GameEngine::NextMoveGetter::ItemOpponentPlayMinion * GameEngine::NextMoveGetter::ItemOpponentPlayMinion::Clone() const
{
	return new ItemOpponentPlayMinion(*this);
}

inline bool GameEngine::NextMoveGetter::ItemOpponentPlayMinion::GetNextMove(Move & move)
{
	if (this->done) return false;

	move.action = Move::ACTION_OPPONENT_PLAY_MINION;
	move.data.opponent_play_minion_data.card = this->playing_card;
	move.data.opponent_play_minion_data.data.put_location = this->put_location;

	if (this->required_targets.None())
	{
		move.data.opponent_play_minion_data.data.target = SLOT_INVALID;
		this->done = true;
	}
	else {
		move.data.opponent_play_minion_data.data.target = this->required_targets.GetOneTarget();
		this->required_targets.ClearOneTarget(move.data.opponent_play_minion_data.data.target);
		if (this->required_targets.None()) this->done = true;
	}
	return true;
}

inline bool GameEngine::NextMoveGetter::ItemOpponentPlayMinion::operator==(ItemOpponentPlayMinion const & rhs) const
{
	if (this->playing_card != rhs.playing_card) return false;
	if (this->put_location != rhs.put_location) return false;
	if (this->required_targets != rhs.required_targets) return false;
	return true;
}

inline bool GameEngine::NextMoveGetter::ItemOpponentPlayMinion::operator!=(ItemOpponentPlayMinion const & rhs) const
{
	return !(*this == rhs);
}

inline GameEngine::NextMoveGetter::ItemPlayerEquipWeapon::ItemPlayerEquipWeapon(
	Hand::Locator hand_card, SlotIndexBitmap required_targets)
	: hand_card(hand_card), required_targets(required_targets)
{
	this->done = false;
}

inline GameEngine::NextMoveGetter::ItemPlayerEquipWeapon * GameEngine::NextMoveGetter::ItemPlayerEquipWeapon::Clone() const
{
	return new ItemPlayerEquipWeapon(*this);
}

inline bool GameEngine::NextMoveGetter::ItemPlayerEquipWeapon::GetNextMove(Move & move)
{
	if (this->done) return false;

	move.action = Move::ACTION_PLAYER_EQUIP_WEAPON;
	move.data.player_equip_weapon_data.hand_card = this->hand_card;

	if (this->required_targets.None())
	{
		move.data.player_equip_weapon_data.data.target = SLOT_INVALID;
		this->done = true;
	}
	else {
		move.data.player_equip_weapon_data.data.target = this->required_targets.GetOneTarget();
		this->required_targets.ClearOneTarget(move.data.player_equip_weapon_data.data.target);
		if (this->required_targets.None()) this->done = true;
	}
	return true;
}

inline bool GameEngine::NextMoveGetter::ItemPlayerEquipWeapon::operator==(ItemPlayerEquipWeapon const & rhs) const
{
	if (this->hand_card != rhs.hand_card) return false;
	if (this->required_targets != rhs.required_targets) return false;
	return true;
}

inline bool GameEngine::NextMoveGetter::ItemPlayerEquipWeapon::operator!=(ItemPlayerEquipWeapon const & rhs) const
{
	return !(*this == rhs);
}

inline GameEngine::NextMoveGetter::ItemOpponentEquipWeapon::ItemOpponentEquipWeapon(
	Card playing_card, SlotIndexBitmap required_targets)
	: playing_card(playing_card), required_targets(required_targets)
{
	this->done = false;
}

inline GameEngine::NextMoveGetter::ItemOpponentEquipWeapon * GameEngine::NextMoveGetter::ItemOpponentEquipWeapon::Clone() const
{
	return new ItemOpponentEquipWeapon(*this);
}

inline bool GameEngine::NextMoveGetter::ItemOpponentEquipWeapon::GetNextMove(Move & move)
{
	if (this->done) return false;

	move.action = Move::ACTION_OPPONENT_EQUIP_WEAPON;
	move.data.opponent_equip_weapon_data.card = this->playing_card;

	if (this->required_targets.None())
	{
		move.data.opponent_equip_weapon_data.data.target = SLOT_INVALID;
		this->done = true;
	}
	else {
		move.data.opponent_equip_weapon_data.data.target = this->required_targets.GetOneTarget();
		this->required_targets.ClearOneTarget(move.data.opponent_equip_weapon_data.data.target);
		if (this->required_targets.None()) this->done = true;
	}
	return true;
}

inline bool GameEngine::NextMoveGetter::ItemOpponentEquipWeapon::operator==(ItemOpponentEquipWeapon const & rhs) const
{
	if (this->playing_card != rhs.playing_card) return false;
	if (this->required_targets != rhs.required_targets) return false;
	return true;
}

inline bool GameEngine::NextMoveGetter::ItemOpponentEquipWeapon::operator!=(ItemOpponentEquipWeapon const & rhs) const
{
	return !(*this == rhs);
}

inline GameEngine::NextMoveGetter::ItemAttack::ItemAttack(SlotIndexBitmap && attacker, SlotIndexBitmap && attacked)
{
	this->attacker = attacker;
	this->attacked = attacked;
	this->attacked_origin = attacked;
}

inline GameEngine::NextMoveGetter::ItemAttack * GameEngine::NextMoveGetter::ItemAttack::Clone() const
{
	return new ItemAttack(*this);
}

inline bool GameEngine::NextMoveGetter::ItemAttack::GetNextMove(Move & move)
{
	if (this->attacker.None()) return false;
	if (this->attacked.None()) {
		this->attacker.ClearOneTarget(this->attacker.GetOneTarget());
		if (this->attacker.None()) return false;

		this->attacked = this->attacked_origin;
	}

	auto const attacker_idx = this->attacker.GetOneTarget();
	auto const attacked_idx = this->attacked.GetOneTarget();

	move.action = Move::ACTION_ATTACK;
	move.data.attack_data.attacker_idx = attacker_idx;
	move.data.attack_data.attacked_idx = attacked_idx;

	this->attacked.ClearOneTarget(attacked_idx);

	return true;
}

inline bool GameEngine::NextMoveGetter::ItemAttack::operator==(ItemAttack const & rhs) const
{
	if (this->attacker != rhs.attacker) return false;
	if (this->attacked != rhs.attacked) return false;
	if (this->attacked_origin != rhs.attacked_origin) return false;
	return true;
}

inline bool GameEngine::NextMoveGetter::ItemAttack::operator!=(ItemAttack const & rhs) const
{
	return !(*this == rhs);
}