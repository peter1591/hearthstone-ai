#pragma once

#include <list>
#include "minion.h"

namespace GameEngine {
namespace BoardObjects {

class Minions;

class MinionsItem
{
	friend std::hash<MinionsItem>;

public:
	MinionsItem(Minion minion, bool pending_removal = false)
		: minion(minion), pending_removal(pending_removal)
	{
	}

	bool operator==(MinionsItem const& rhs) const
	{
		if (this->pending_removal != rhs.pending_removal) return false;
		if (this->minion != rhs.minion) return false;
		return true;
	}

	bool operator!=(MinionsItem const& rhs) const { return !(*this == rhs); }

	Minion & Get() { return this->minion; }
	Minion const& Get() const { return this->minion; }

	void SetPendingRemoval() { this->pending_removal = true; }
	bool IsPendingRemoval() const { return this->pending_removal; }

private:
	Minion minion;
	bool pending_removal;
};

class MinionsIteratorWithIndex
{
public:
	typedef std::list<MinionsItem> container_type;

public:
	MinionsIteratorWithIndex(SlotIndex slot_idx_begin, container_type::iterator it_begin, Minions &container)
		: slot_idx(slot_idx_begin), it(it_begin), container(container) { }

	bool IsEnd() const;

	void GoToNext();
	void EraseAndGoToNext();
	MinionsIteratorWithIndex InsertBefore(Minion && minion);

	bool IsPendingRemoval() const;
	void MarkPendingRemoval();

	Minion* operator->() { return &(this->it->Get()); }
	Minion const* operator->() const { return &(this->it->Get()) ; }
	Minion& operator*() { return this->it->Get(); }
	Minion const& operator*() const { return this->it->Get(); }

	Minions & GetOwner() const { return this->container; }
	SlotIndex GetSlotIdx() const { return this->slot_idx; }

public: // hooks
	void HookAfterMinionAdded(Board & board, MinionsIteratorWithIndex & added_minion) {
		this->it->Get().HookAfterMinionAdded(board, *this, added_minion);
	}

private:
	SlotIndex slot_idx;
	container_type::iterator it;
	Minions & container;
};

class MinionsConstIteratorWithIndex
{
public:
	typedef std::list<MinionsItem> container_type;

public:
	MinionsConstIteratorWithIndex(SlotIndex slot_idx_begin, container_type::const_iterator it_begin, Minions const& container)
		: slot_idx(slot_idx_begin), it(it_begin), container(container) { }

	bool IsEnd() const;
	void GoToNext();

	bool IsPendingRemoval() const;

	Minion const* operator->() { return &(this->it->Get()); }
	Minion const* operator->() const { return &(this->it->Get()); }
	Minion const& operator*() { return this->it->Get(); }
	Minion const& operator*() const { return this->it->Get(); }

	Minions const& GetOwner() const { return this->container; }
	SlotIndex GetSlotIdx() const { return this->slot_idx; }

private:
	SlotIndex slot_idx;
	container_type::const_iterator it;
	Minions const& container;
};

} // BoardObjects
} // GameEngine

namespace std {
	template <> struct hash<GameEngine::BoardObjects::MinionsItem> {
		typedef GameEngine::BoardObjects::MinionsItem argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			GameEngine::hash_combine(result, s.pending_removal);
			GameEngine::hash_combine(result, s.minion);

			return result;
		}
	};
}