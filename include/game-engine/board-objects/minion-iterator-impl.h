#pragma once

#include "minion-iterator.h"

namespace GameEngine {
	inline MinionConstIteratorWithSlotIndex::MinionConstIteratorWithSlotIndex(Minions const & minions, container_type::const_iterator it_begin, SlotIndex slot_idx_begin)
		: minions(minions), it(it_begin), slot_idx(slot_idx_begin)
	{
#ifdef DEBUG
		this->container_change_id = this->minions.change_id;
#endif
	}

	inline SlotIndex GameEngine::MinionConstIteratorWithSlotIndex::GetSlotIdx() const
	{
		this->CheckChangeId();
		return this->slot_idx;
	}

	inline MinionConstIteratorWithSlotIndex::const_iterator::pointer MinionConstIteratorWithSlotIndex::operator->()
	{
		return this->it.operator->();
	}

	inline MinionConstIteratorWithSlotIndex::const_iterator::reference & MinionConstIteratorWithSlotIndex::operator*()
	{
		return this->it.operator*();
	}

	inline void GameEngine::MinionConstIteratorWithSlotIndex::CheckChangeId() const
	{
#ifdef DEBUG
		if (this->container_change_id != this->minions.change_id) {
			throw std::runtime_error("container changed when iterator is fetched.");
		}
#endif
	}

	inline void MinionConstIteratorWithSlotIndex::GoToNext()
	{
		this->CheckChangeId();
		if (!this->it->GetMinion().pending_removal) SlotIndexHelper::Increase(this->slot_idx);
		this->it++;
	}

	inline bool MinionConstIteratorWithSlotIndex::IsEnd() const
	{
		this->CheckChangeId();
		return this->it == this->minions.end();
	}


	inline MinionIterator::MinionIterator(Board & board, Minions & minions, container_type::iterator it)
		: board(board), minions(minions), it(it)
	{
#ifdef DEBUG
		if (it != minions.end()) {
			if (&it->GetBoard() != &board) throw std::runtime_error("consistency check failed");
			if (&it->GetMinions() != &minions) throw std::runtime_error("consistency check failed");
		}

		this->container_change_id = this->minions.change_id;
#endif
	}

	inline Board & MinionIterator::GetBoard() const
	{
		this->CheckChangeId();
		return this->board;
	}

	inline Minions & MinionIterator::GetMinions() const
	{
		this->CheckChangeId();
		return this->minions;
	}

	inline MinionIterator::container_type::iterator MinionIterator::GetIterator() const
	{
		this->CheckChangeId();
		return it;
	}

	inline bool MinionIterator::IsPlayerSide() const
	{
		this->CheckChangeId();
		return &this->minions == &this->board.player.minions;
	}

	inline bool MinionIterator::IsOpponentSide() const
	{
		this->CheckChangeId();
		return &this->minions == &this->board.opponent.minions;
	}

	inline bool MinionIterator::IsBegin() const
	{
		this->CheckChangeId();
		return this->it == this->minions.begin();
	}

	inline MinionIterator & MinionIterator::GoToPrevious()
	{
		this->CheckChangeId();
		this->it--;
		return *this;
	}

	inline void MinionIterator::CheckChangeId() const
	{
#ifdef DEBUG
		if (this->container_change_id != this->minions.change_id) {
			throw std::runtime_error("container changed when iterator is fetched.");
		}
#endif
	}

	inline MinionIterator & MinionIterator::GoToNext()
	{
		this->CheckChangeId();
		this->it++;
		return *this;
	}

	inline bool MinionIterator::IsEnd() const
	{
		this->CheckChangeId();
		return this->it == this->minions.end();
	}

	inline MinionIterator::iterator::pointer MinionIterator::operator->()
	{
		return this->it.operator->();
	}

	inline MinionIterator::iterator::reference MinionIterator::operator*()
	{
		return this->it.operator*();
	}
} // GameEngine
