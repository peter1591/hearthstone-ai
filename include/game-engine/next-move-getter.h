#pragma once

#include <vector>
#include "move.h"
#include "targetor.h"

namespace GameEngine
{

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
		ItemGetMoves* Clone() const { return new ItemGetMoves(*this); }

		bool GetNextMove(Move &move)
		{
			if (this->moves.empty()) return false;
			move = this->moves.back();
			this->moves.pop_back();
			return true;
		}

		std::vector<Move> moves;

		bool operator==(ItemGetMoves const& rhs) const {
			if (this->moves != rhs.moves) return false;
			return true;
		}

		bool operator!=(ItemGetMoves const& rhs) const {
			return !(*this == rhs);
		}
	};

	class ItemGetMove : public ItemBase
	{
	public:
		ItemGetMove(Move const& rhs) : empty(false) { this->move = rhs; }

		ItemGetMove* Clone() const { return new ItemGetMove(*this); }

		bool GetNextMove(Move & move)
		{
			if (this->empty) return false;
			move = this->move;
			this->empty = true;
			return true;
		}

		bool operator==(ItemGetMove const& rhs) const {
			if (this->empty != rhs.empty) return false;
			if (this->move != rhs.move) return false;
			return true;
		}

		bool operator!=(ItemGetMove const& rhs) const {
			return !(*this == rhs);
		}

	private:
		Move move;
		bool empty;
	};

	class ItemPlayerAttack : public ItemBase
	{
	public:
		ItemPlayerAttack(TargetorBitmap && attacker, TargetorBitmap && attacked)
		{
			this->attacker = attacker;
			this->attacked_origin = attacked;
		}

		ItemPlayerAttack* Clone() const { return new ItemPlayerAttack(*this); }

		bool GetNextMove(Move & move)
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

		bool operator==(ItemPlayerAttack const& rhs) const {
			if (this->attacker != rhs.attacker) return false;
			if (this->attacked != rhs.attacked) return false;
			if (this->attacked_origin != rhs.attacked_origin) return false;
			return true;
		}

		bool operator!=(ItemPlayerAttack const& rhs) const {
			return !(*this == rhs);
		}
	
	private:
		TargetorBitmap attacker;
		TargetorBitmap attacked;
		TargetorBitmap attacked_origin;
	};

public:
	NextMoveGetter() : is_cached_move_valid(false) {}
	~NextMoveGetter()
	{
		for (auto it = this->items.begin(); it != this->items.end(); ++it) {
			delete *it;
		}
	}

	NextMoveGetter(NextMoveGetter const& rhs)
	{
		*this = rhs;
	}

	NextMoveGetter & operator=(NextMoveGetter const& rhs) 
	{
		this->Clear();
		for (auto it = rhs.items.begin(); it != rhs.items.end(); ++it) {
			this->items.push_back((*it)->Clone());
		}
		this->is_cached_move_valid = rhs.is_cached_move_valid;
		if (this->is_cached_move_valid) this->cached_move = rhs.cached_move;
		return *this;
	}

	bool GetNextMove(Move &move)
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

	bool Empty() 
	{
		if (this->is_cached_move_valid) return false;
		if (this->GetNextMove(this->cached_move) == false) return true;

		this->is_cached_move_valid = true;
		return false;
	}

	void Clear()
	{
		for (auto it = this->items.begin(); it != this->items.end();) {
			delete *it;
			it = this->items.erase(it);
		}
		this->is_cached_move_valid = false;
	}

	bool operator==(NextMoveGetter const& rhs) const
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
		}

		return true;
	}

	bool operator!=(NextMoveGetter const& rhs) const
	{
		return !(*this == rhs);
	}

public:
	std::vector<ItemBase*> items;

private:
	template <typename T>
	static bool IsItemEqual(ItemBase* lhs, ItemBase* rhs)
	{
		auto const& lhs_item = dynamic_cast<T*>(lhs);

		if (lhs_item != nullptr) {
			auto const& rhs_item = dynamic_cast<T*>(rhs);
			if (rhs_item == nullptr) return false;
			if (*lhs_item != *rhs_item) return false;
		}
		return true;
	}

private:
	bool is_cached_move_valid;
	Move cached_move;
};

}