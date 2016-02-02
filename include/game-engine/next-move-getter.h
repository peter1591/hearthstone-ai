#pragma once

#include <vector>
#include "move.h"
#include "targetor.h"

namespace GameEngine {

class NextMoveGetter
{
public:
	class ItemGetMoves
	{
	public:
		ItemGetMoves* Clone() const;
		bool GetNextMove(Move &move);
		bool operator==(ItemGetMoves const& rhs) const;
		bool operator!=(ItemGetMoves const& rhs) const;

	public:
		std::vector<Move> moves;
	};

	class ItemGetMove
	{
	public:
		ItemGetMove(Move && rhs);
		ItemGetMove* Clone() const;
		bool GetNextMove(Move & move);
		bool operator==(ItemGetMove const& rhs) const;
		bool operator!=(ItemGetMove const& rhs) const;

	private:
		Move move;
		bool empty;
	};

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

	class ItemPlayerAttack
	{
	public:
		ItemPlayerAttack(TargetorBitmap && attacker, TargetorBitmap && attacked);
		ItemPlayerAttack* Clone() const;
		bool GetNextMove(Move & move);
		bool operator==(ItemPlayerAttack const& rhs) const;
		bool operator!=(ItemPlayerAttack const& rhs) const;
	
	private:
		TargetorBitmap attacker;
		TargetorBitmap attacked;
		TargetorBitmap attacked_origin;
	};

public:
	NextMoveGetter();

	void AddItem(ItemGetMove && items);
	void AddItem(ItemGetMoves && items);
	void AddItem(ItemPlayerAttack && items);
	void AddItem(ItemPlayerPlayMinion && items);
	void AddItems(std::vector<ItemGetMove> && items);
	void AddItems(std::vector<ItemGetMoves> && items);
	void AddItems(std::vector<ItemPlayerAttack> && items);
	void AddItems(std::vector<ItemPlayerPlayMinion> && items);

	bool GetNextMove(Move &move);
	bool Empty();

	bool operator==(NextMoveGetter const& rhs) const;
	bool operator!=(NextMoveGetter const& rhs) const;

private:
	template <typename T> bool GetNextMoveFromContainer(std::vector<T> & container, Move &move);
	template <typename T> static bool IsEqual(std::vector<T> const& lhs, std::vector<T> const& rhs);

private:
	std::vector<ItemGetMove> items_get_move;
	std::vector<ItemGetMoves> items_get_moves;
	std::vector<ItemPlayerAttack> items_player_attack;
	std::vector<ItemPlayerPlayMinion> items_player_play_minion;

	bool is_cached_move_valid;
	Move cached_move;
};

} // namespace GameEngine

inline GameEngine::NextMoveGetter::NextMoveGetter()
	: is_cached_move_valid(false)
{
}

template<typename T>
inline bool GameEngine::NextMoveGetter::IsEqual(std::vector<T> const & lhs, std::vector<T> const & rhs)
{
	if (lhs.size() != rhs.size()) return false;
	for (size_t i = 0; i < lhs.size(); ++i)
	{
		if (lhs[i] != rhs[i]) return false;
	}
	return true;
}

inline void GameEngine::NextMoveGetter::AddItem(GameEngine::NextMoveGetter::ItemGetMove && items)
{
	this->items_get_move.push_back(std::move(items));
}

inline void GameEngine::NextMoveGetter::AddItems(std::vector<GameEngine::NextMoveGetter::ItemGetMove> && items)
{
	this->items_get_move.insert(
		this->items_get_move.end(),
		std::make_move_iterator(items.begin()),
		std::make_move_iterator(items.end()));
}

inline void GameEngine::NextMoveGetter::AddItem(GameEngine::NextMoveGetter::ItemGetMoves && items)
{
	this->items_get_moves.push_back(std::move(items));
}

inline void GameEngine::NextMoveGetter::AddItems(std::vector<GameEngine::NextMoveGetter::ItemGetMoves> && items)
{
	this->items_get_moves.insert(
		this->items_get_moves.end(),
		std::make_move_iterator(items.begin()),
		std::make_move_iterator(items.end()));
}

inline void GameEngine::NextMoveGetter::AddItem(GameEngine::NextMoveGetter::ItemPlayerAttack && items)
{
	this->items_player_attack.push_back(std::move(items));
}

inline void GameEngine::NextMoveGetter::AddItems(std::vector<GameEngine::NextMoveGetter::ItemPlayerAttack> && items)
{
	this->items_player_attack.insert(
		this->items_player_attack.end(),
		std::make_move_iterator(items.begin()),
		std::make_move_iterator(items.end()));
}

inline void GameEngine::NextMoveGetter::AddItem(GameEngine::NextMoveGetter::ItemPlayerPlayMinion && items)
{
	this->items_player_play_minion.push_back(std::move(items));
}

inline void GameEngine::NextMoveGetter::AddItems(std::vector<GameEngine::NextMoveGetter::ItemPlayerPlayMinion> && items)
{
	this->items_player_play_minion.insert(
		this->items_player_play_minion.end(),
		std::make_move_iterator(items.begin()),
		std::make_move_iterator(items.end()));
}

template<typename T>
inline bool GameEngine::NextMoveGetter::GetNextMoveFromContainer(std::vector<T>& container, Move &move)
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

	if (this->GetNextMoveFromContainer(this->items_get_move, move)) return true;
	if (this->GetNextMoveFromContainer(this->items_get_moves, move)) return true;
	if (this->GetNextMoveFromContainer(this->items_player_attack, move)) return true;
	if (this->GetNextMoveFromContainer(this->items_player_play_minion, move)) return true;
	return false;
}

inline bool GameEngine::NextMoveGetter::operator==(NextMoveGetter const & rhs) const
{
	if (!IsEqual(this->items_get_move, rhs.items_get_move)) return false;
	if (!IsEqual(this->items_get_moves, rhs.items_get_moves)) return false;
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

inline GameEngine::NextMoveGetter::ItemGetMoves * GameEngine::NextMoveGetter::ItemGetMoves::Clone() const
{
	return new ItemGetMoves(*this);
}

inline bool GameEngine::NextMoveGetter::ItemGetMoves::GetNextMove(Move & move)
{
	if (this->moves.empty()) return false;
	move = this->moves.back();
	this->moves.pop_back();
	return true;
}

inline bool GameEngine::NextMoveGetter::ItemGetMoves::operator==(ItemGetMoves const & rhs) const
{
	if (this->moves != rhs.moves) return false;
	return true;
}

inline bool GameEngine::NextMoveGetter::ItemGetMoves::operator!=(ItemGetMoves const & rhs) const
{
	return !(*this == rhs);
}

inline GameEngine::NextMoveGetter::ItemGetMove::ItemGetMove(Move && rhs)
	: empty(false) 
{
	this->move = rhs;
}

inline GameEngine::NextMoveGetter::ItemGetMove * GameEngine::NextMoveGetter::ItemGetMove::Clone() const
{
	return new ItemGetMove(*this);
}

inline bool GameEngine::NextMoveGetter::ItemGetMove::GetNextMove(Move & move)
{
	if (this->empty) return false;
	move = this->move;
	this->empty = true;
	return true;
}

inline bool GameEngine::NextMoveGetter::ItemGetMove::operator==(ItemGetMove const & rhs) const
{
	if (this->empty != rhs.empty) return false;
	if (this->move != rhs.move) return false;
	return true;
}

inline bool GameEngine::NextMoveGetter::ItemGetMove::operator!=(ItemGetMove const & rhs) const
{
	return !(*this == rhs);
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

inline GameEngine::NextMoveGetter::ItemPlayerAttack::ItemPlayerAttack(TargetorBitmap && attacker, TargetorBitmap && attacked)
{
	this->attacker = attacker;
	this->attacked_origin = attacked;
}

inline GameEngine::NextMoveGetter::ItemPlayerAttack * GameEngine::NextMoveGetter::ItemPlayerAttack::Clone() const
{
	return new ItemPlayerAttack(*this);
}

inline bool GameEngine::NextMoveGetter::ItemPlayerAttack::GetNextMove(Move & move)
{
	if (this->attacker.None()) return false;
	if (this->attacked.None()) {
		this->attacker.ClearOneTarget(this->attacker.GetOneTarget());
		if (this->attacker.None()) return false;

		this->attacked = this->attacked_origin;
	}

	size_t attacker_idx = this->attacker.GetOneTarget();
	size_t attacked_idx = this->attacked.GetOneTarget();

	move.action = Move::ACTION_PLAYER_ATTACK;
	move.data.player_attack_data.attacker_idx = attacker_idx;
	move.data.player_attack_data.attacked_idx = attacked_idx;

	this->attacked.ClearOneTarget(attacked_idx);

	return true;
}

inline bool GameEngine::NextMoveGetter::ItemPlayerAttack::operator==(ItemPlayerAttack const & rhs) const
{
	if (this->attacker != rhs.attacker) return false;
	if (this->attacked != rhs.attacked) return false;
	if (this->attacked_origin != rhs.attacked_origin) return false;
	return true;
}

inline bool GameEngine::NextMoveGetter::ItemPlayerAttack::operator!=(ItemPlayerAttack const & rhs) const
{
	return !(*this == rhs);
}