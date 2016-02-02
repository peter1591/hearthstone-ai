#pragma once

#include <vector>
#include <list>
#include "move.h"
#include "targetor.h"

namespace GameEngine {

class NextMoveGetter
{
public:
	class ItemPlayerPlayMinion
	{
	public:
		ItemPlayerPlayMinion(Hand::Locator hand_card, int put_locations_min, int put_locations_max);
		ItemPlayerPlayMinion* Clone() const;
		bool GetNextMove(Move & move);
		bool operator==(ItemPlayerPlayMinion const& rhs) const;
		bool operator!=(ItemPlayerPlayMinion const& rhs) const;

	private:
		Hand::Locator hand_card;
		bool hand_card_played;

		int put_locations_min;
		int put_locations_max;
		int put_locations_current;
	};

	class ItemAttack
	{
	public:
		ItemAttack(TargetorBitmap && attacker, TargetorBitmap && attacked);
		ItemAttack* Clone() const;
		bool GetNextMove(Move & move);
		bool operator==(ItemAttack const& rhs) const;
		bool operator!=(ItemAttack const& rhs) const;
	
	private:
		TargetorBitmap attacker;
		TargetorBitmap attacked;
		TargetorBitmap attacked_origin;
	};

public:
	NextMoveGetter();

	void AddItem(Move const& move);
	void AddItem(Move && move);
	void AddItem(ItemAttack && items);
	void AddItem(ItemPlayerPlayMinion && items);
	void AddItems(std::list<Move> && items);
	void AddItems(std::list<ItemAttack> && items);
	void AddItems(std::list<ItemPlayerPlayMinion> && items);

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

	bool is_cached_move_valid;
	Move cached_move;
};

} // namespace GameEngine

inline GameEngine::NextMoveGetter::NextMoveGetter()
	: is_cached_move_valid(false)
{
}

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
	if (this->is_cached_move_valid)
	{
		move = this->cached_move;
		this->is_cached_move_valid = false;
		return true;
	}

	if (!this->moves.empty()) {
		move = this->moves.back();
		this->moves.pop_back();
		return true;
	}

	if (this->GetNextMoveFromContainer(this->items_player_attack, move)) return true;
	if (this->GetNextMoveFromContainer(this->items_player_play_minion, move)) return true;
	return false;
}

inline bool GameEngine::NextMoveGetter::operator==(NextMoveGetter const & rhs) const
{
	if (!IsEqual(this->moves, rhs.moves)) return false;
	if (!IsEqual(this->items_player_attack, rhs.items_player_attack)) return false;
	if (!IsEqual(this->items_player_play_minion, rhs.items_player_play_minion)) return false;
	return true;
}

inline bool GameEngine::NextMoveGetter::operator!=(NextMoveGetter const & rhs) const
{
	return !(*this == rhs);
}

inline bool GameEngine::NextMoveGetter::Empty()
{
	if (this->is_cached_move_valid) return false;
	if (this->GetNextMove(this->cached_move) == false) return true;

	this->is_cached_move_valid = true;
	return false;
}

inline GameEngine::NextMoveGetter::ItemPlayerPlayMinion::ItemPlayerPlayMinion(
	Hand::Locator hand_card, int put_locations_min, int put_locations_max)
	: hand_card(hand_card), put_locations_min(put_locations_min), put_locations_max(put_locations_max)
{
	this->put_locations_current = this->put_locations_min;
	this->hand_card_played = false;
}

inline GameEngine::NextMoveGetter::ItemPlayerPlayMinion * GameEngine::NextMoveGetter::ItemPlayerPlayMinion::Clone() const
{
	return new ItemPlayerPlayMinion(*this);
}

inline bool GameEngine::NextMoveGetter::ItemPlayerPlayMinion::GetNextMove(Move & move)
{
	if (this->hand_card_played) return false;

#ifdef CHOOSE_WHERE_TO_PUT_MINION
	if (this->put_locations_current > this->put_locations_max) {
		this->hand_card_played = true;
		return false;
	}
#endif

	move.action = Move::ACTION_PLAY_HAND_CARD_MINION;
	move.data.play_hand_card_minion_data.hand_card = this->hand_card;

#ifdef CHOOSE_WHERE_TO_PUT_MINION
	move.data.play_hand_card_minion_data.location = this->put_locations_current;
	++this->put_locations_current;
#else
	this->hand_card_played = true;
#endif
}

inline bool GameEngine::NextMoveGetter::ItemPlayerPlayMinion::operator==(ItemPlayerPlayMinion const & rhs) const
{
	if (this->hand_card != rhs.hand_card) return false;
	if (this->put_locations_min != rhs.put_locations_min) return false;
	if (this->put_locations_max != rhs.put_locations_max) return false;
	if (this->put_locations_current != rhs.put_locations_current) return false;
	return true;
}

inline bool GameEngine::NextMoveGetter::ItemPlayerPlayMinion::operator!=(ItemPlayerPlayMinion const & rhs) const
{
	return !(*this == rhs);
}

inline GameEngine::NextMoveGetter::ItemAttack::ItemAttack(TargetorBitmap && attacker, TargetorBitmap && attacked)
{
	this->attacker = attacker;
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

	size_t attacker_idx = this->attacker.GetOneTarget();
	size_t attacked_idx = this->attacked.GetOneTarget();

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