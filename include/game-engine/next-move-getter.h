#pragma once

#include <vector>
#include "move.h"
#include "targetor.h"

namespace GameEngine {

class NextMoveGetter
{
public:
	class ItemBase
	{
	public:
		virtual ItemBase* Clone() const = 0;
		virtual bool GetNextMove(Move &move) = 0;
	};

	class ItemGetMoves : public ItemBase
	{
	public:
		ItemGetMoves* Clone() const;
		bool GetNextMove(Move &move);
		bool operator==(ItemGetMoves const& rhs) const;
		bool operator!=(ItemGetMoves const& rhs) const;

	public:
		std::vector<Move> moves;
	};

	class ItemGetMove : public ItemBase
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

	class ItemPlayerPlayMinion : public ItemBase
	{
	public:
		ItemPlayerPlayMinion(Hand::CardsBitmap hand_cards, int put_locations_min, int put_locations_max);
		ItemPlayerPlayMinion* Clone() const;
		bool GetNextMove(Move & move);
		bool operator==(ItemPlayerPlayMinion const& rhs) const;
		bool operator!=(ItemPlayerPlayMinion const& rhs) const;

	private:
		Hand::CardsBitmap hand_cards;
		int put_locations_min;
		int put_locations_max;
		int put_locations_current;
	};

	class ItemPlayerAttack : public ItemBase
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
	~NextMoveGetter();

	NextMoveGetter(NextMoveGetter const& rhs);
	NextMoveGetter & operator=(NextMoveGetter const& rhs);

	bool operator==(NextMoveGetter const& rhs) const;
	bool operator!=(NextMoveGetter const& rhs) const;

	bool GetNextMove(Move &move);
	bool Empty();
	void Clear();

public:
	std::vector<ItemBase*> items;

private:
	template <typename T> static bool IsItemEqual(ItemBase* lhs, ItemBase* rhs);

private:
	bool is_cached_move_valid;
	Move cached_move;
};

} // namespace GameEngine

inline GameEngine::NextMoveGetter::NextMoveGetter()
	: is_cached_move_valid(false)
{
}

inline GameEngine::NextMoveGetter::~NextMoveGetter()
{
	for (auto it = this->items.begin(); it != this->items.end(); ++it) {
		delete *it;
	}
}

inline GameEngine::NextMoveGetter::NextMoveGetter(NextMoveGetter const & rhs)
{
	*this = rhs;
}

inline GameEngine::NextMoveGetter & GameEngine::NextMoveGetter::operator=(NextMoveGetter const & rhs)
{
	this->Clear();
	for (auto it = rhs.items.begin(); it != rhs.items.end(); ++it) {
		this->items.push_back((*it)->Clone());
	}
	this->is_cached_move_valid = rhs.is_cached_move_valid;
	if (this->is_cached_move_valid) this->cached_move = rhs.cached_move;
	return *this;
}

template<typename T>
inline bool GameEngine::NextMoveGetter::IsItemEqual(ItemBase * lhs, ItemBase * rhs)
{
	auto const& lhs_item = dynamic_cast<T*>(lhs);

	if (lhs_item != nullptr) {
		auto const& rhs_item = dynamic_cast<T*>(rhs);
		if (rhs_item == nullptr) return false;
		if (*lhs_item != *rhs_item) return false;
	}
	return true;
}

inline bool GameEngine::NextMoveGetter::operator==(NextMoveGetter const & rhs) const
{
	auto const& items_count = this->items.size();
	if (items_count != rhs.items.size()) return false;
	if (this->is_cached_move_valid != rhs.is_cached_move_valid) return false;
	if (this->is_cached_move_valid)
		if (this->cached_move != rhs.cached_move) return false;

	for (auto i = 0; i < items_count; ++i)
	{
		if (IsItemEqual<ItemGetMoves>(this->items[i], rhs.items[i]) == false) return false;
		if (IsItemEqual<ItemGetMove>(this->items[i], rhs.items[i]) == false) return false;
		if (IsItemEqual<ItemPlayerAttack>(this->items[i], rhs.items[i]) == false) return false;
	}

	return true;
}

inline bool GameEngine::NextMoveGetter::operator!=(NextMoveGetter const & rhs) const
{
	return !(*this == rhs);
}

inline bool GameEngine::NextMoveGetter::GetNextMove(Move & move)
{
	if (this->is_cached_move_valid)
	{
		move = this->cached_move;
		this->is_cached_move_valid = false;
		return true;
	}

	while (this->items.empty() == false)
	{
		auto &item = this->items.back();
		if (item->GetNextMove(move)) return true;
		this->items.pop_back();
	}
	return false;
}

inline bool GameEngine::NextMoveGetter::Empty()
{
	if (this->is_cached_move_valid) return false;
	if (this->GetNextMove(this->cached_move) == false) return true;

	this->is_cached_move_valid = true;
	return false;
}

inline void GameEngine::NextMoveGetter::Clear()
{
	for (auto it = this->items.begin(); it != this->items.end();) {
		delete *it;
		it = this->items.erase(it);
	}
	this->is_cached_move_valid = false;
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
	Hand::CardsBitmap hand_cards, int put_locations_min, int put_locations_max)
	: hand_cards(hand_cards), put_locations_min(put_locations_min), put_locations_max(put_locations_max)
{
	this->put_locations_current = 0;
}

inline GameEngine::NextMoveGetter::ItemPlayerPlayMinion * GameEngine::NextMoveGetter::ItemPlayerPlayMinion::Clone() const
{
	return new ItemPlayerPlayMinion(*this);
}

inline bool GameEngine::NextMoveGetter::ItemPlayerPlayMinion::GetNextMove(Move & move)
{
	if (this->hand_cards.None()) return false;

#ifdef CHOOSE_WHERE_TO_PUT_MINION
	if (this->put_locations_current >= this->put_locations_max) {
		this->hand_cards.ClearOneCard(this->hand_cards.GetOneCard());
		if (this->hand_cards.None()) return false;
		this->put_locations_current = this->put_locations_min;
	}
#endif

	move.action = Move::ACTION_PLAY_HAND_CARD_MINION;
	move.data.play_hand_card_minion_data.hand_card = this->hand_cards.GetOneCard();

#ifdef CHOOSE_WHERE_TO_PUT_MINION
	move.data.play_hand_card_minion_data.location = this->put_locations_current;
	++this->put_locations_current;
#else
	this->hand_cards.ClearOneCard(move.data.play_hand_card_minion_data.hand_card);
#endif
}

inline bool GameEngine::NextMoveGetter::ItemPlayerPlayMinion::operator==(ItemPlayerPlayMinion const & rhs) const
{
	if (this->hand_cards != rhs.hand_cards) return false;
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