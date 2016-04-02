#pragma once

#include "next-move-getter.h"

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

inline void GameEngine::NextMoveGetter::AddItem(GameEngine::NextMoveGetter::ItemPlayHandMinion && item)
{
	this->items_play_hand_minion.push_back(std::move(item));
}

inline void GameEngine::NextMoveGetter::AddItems(std::list<GameEngine::NextMoveGetter::ItemPlayHandMinion> && items)
{
	this->items_play_hand_minion.splice(this->items_play_hand_minion.end(), std::move(items));
}

inline void GameEngine::NextMoveGetter::AddItem(GameEngine::NextMoveGetter::ItemPlayHandWeapon && item)
{
	this->items_play_hand_weapon.push_back(std::move(item));
}

inline void GameEngine::NextMoveGetter::AddItems(std::list<GameEngine::NextMoveGetter::ItemPlayHandWeapon> && items)
{
	this->items_play_hand_weapon.splice(this->items_play_hand_weapon.end(), std::move(items));
}

template<typename T>
inline bool GameEngine::NextMoveGetter::GetNextMoveFromContainer(std::list<T>& container, Board const& board, Move &move)
{
	while (!container.empty())
	{
		T & item = container.back();
		if (item.GetNextMove(board, move)) return true;
		container.pop_back();
	}
	return false;
}

inline bool GameEngine::NextMoveGetter::GetNextMove(GameEngine::Board const& board, Move & move)
{
	if (!this->moves.empty()) {
		move = this->moves.back();
		this->moves.pop_back();
		return true;
	}

	if (this->GetNextMoveFromContainer(this->items_player_attack, board, move)) return true;
	if (this->GetNextMoveFromContainer(this->items_play_hand_minion, board, move)) return true;
	if (this->GetNextMoveFromContainer(this->items_play_hand_weapon, board, move)) return true;
	return false;
}

inline void GameEngine::NextMoveGetter::Clear()
{
	this->moves.clear();
	this->items_player_attack.clear();
	this->items_play_hand_minion.clear();
	this->items_play_hand_weapon.clear();
}

inline bool GameEngine::NextMoveGetter::operator==(NextMoveGetter const & rhs) const
{
	if (!IsEqual(this->moves, rhs.moves)) return false;
	if (!IsEqual(this->items_player_attack, rhs.items_player_attack)) return false;
	if (!IsEqual(this->items_play_hand_minion, rhs.items_play_hand_minion)) return false;
	if (!IsEqual(this->items_play_hand_weapon, rhs.items_play_hand_weapon)) return false;
	return true;
}

inline bool GameEngine::NextMoveGetter::operator!=(NextMoveGetter const & rhs) const
{
	return !(*this == rhs);
}

inline GameEngine::NextMoveGetter::ItemPlayHandMinion::ItemPlayHandMinion(
	Player const& player, Hand::Locator hand_card, SlotIndex put_location, SlotIndexBitmap required_targets)
	: player(player), hand_card(hand_card), put_location(put_location), required_targets(required_targets)
{
	this->done = false;
}

inline GameEngine::NextMoveGetter::ItemPlayHandMinion * GameEngine::NextMoveGetter::ItemPlayHandMinion::Clone() const
{
	return new ItemPlayHandMinion(*this);
}

inline bool GameEngine::NextMoveGetter::ItemPlayHandMinion::GetNextMove(GameEngine::Board const& board, Move & move)
{
	if (this->done) return false;

	auto const& playing_card = this->player.hand.GetCard(this->hand_card);

	move.action = Move::ACTION_PLAY_HAND_MINION;
	move.data.play_hand_minion_data.hand_card = this->hand_card;
	move.data.play_hand_minion_data.card_id = playing_card.id;
	move.data.play_hand_minion_data.data.put_location = this->put_location;

	if (this->required_targets.None())
	{
		move.data.play_hand_minion_data.data.target = SLOT_INVALID;
		this->done = true;
	}
	else {
		move.data.play_hand_minion_data.data.target = this->required_targets.GetOneTarget();
		this->required_targets.ClearOneTarget(move.data.play_hand_minion_data.data.target);
		if (this->required_targets.None()) this->done = true;
	}
	return true;
}

inline bool GameEngine::NextMoveGetter::ItemPlayHandMinion::operator==(ItemPlayHandMinion const & rhs) const
{
	if (this->hand_card != rhs.hand_card) return false;
	if (this->put_location != rhs.put_location) return false;
	if (this->required_targets != rhs.required_targets) return false;
	return true;
}

inline bool GameEngine::NextMoveGetter::ItemPlayHandMinion::operator!=(ItemPlayHandMinion const & rhs) const
{
	return !(*this == rhs);
}

inline GameEngine::NextMoveGetter::ItemPlayHandWeapon::ItemPlayHandWeapon(
	Player const& player, Hand::Locator hand_card, SlotIndexBitmap required_targets)
	: player(player), hand_card(hand_card), required_targets(required_targets)
{
	this->done = false;
}

inline GameEngine::NextMoveGetter::ItemPlayHandWeapon * GameEngine::NextMoveGetter::ItemPlayHandWeapon::Clone() const
{
	return new ItemPlayHandWeapon(*this);
}

inline bool GameEngine::NextMoveGetter::ItemPlayHandWeapon::GetNextMove(GameEngine::Board const& board, Move & move)
{
	if (this->done) return false;

	auto const& playing_card = this->player.hand.GetCard(this->hand_card);

	move.action = Move::ACTION_PLAY_HAND_WEAPON;
	move.data.play_hand_weapon_data.hand_card = this->hand_card;
	move.data.play_hand_weapon_data.card_id = playing_card.id;

	if (this->required_targets.None())
	{
		move.data.play_hand_weapon_data.data.target = SLOT_INVALID;
		this->done = true;
	}
	else {
		move.data.play_hand_weapon_data.data.target = this->required_targets.GetOneTarget();
		this->required_targets.ClearOneTarget(move.data.play_hand_weapon_data.data.target);
		if (this->required_targets.None()) this->done = true;
	}
	return true;
}

inline bool GameEngine::NextMoveGetter::ItemPlayHandWeapon::operator==(ItemPlayHandWeapon const & rhs) const
{
	if (this->hand_card != rhs.hand_card) return false;
	if (this->required_targets != rhs.required_targets) return false;
	return true;
}

inline bool GameEngine::NextMoveGetter::ItemPlayHandWeapon::operator!=(ItemPlayHandWeapon const & rhs) const
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

inline bool GameEngine::NextMoveGetter::ItemAttack::GetNextMove(GameEngine::Board const& board, Move & move)
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