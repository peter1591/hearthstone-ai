#pragma once

#include <list>
#include "minion.h"

namespace GameEngine {
namespace BoardObjects {

class Minions;

class MinionIterator
{
	friend class Minions;

public:
	typedef std::list<Minion> container_type;
	typedef container_type::iterator iterator;

private: // only Minions can create instance
	MinionIterator(Board & board, Minions & minions, iterator it);

public:
	Board & GetBoard() const;
	Minions & GetMinions() const;
	iterator GetIterator() const;

	bool IsPlayerSide() const;
	bool IsOpponentSide() const;

	bool IsBegin() const;
	MinionIterator& GoToPrevious();

	MinionIterator& GoToNext();
	bool IsEnd() const;

	iterator::pointer operator->();
	iterator::reference operator*();

private:
	void CheckChangeId() const;

private:
	Board & board;
	Minions & minions;
	iterator it;

#ifdef DEBUG
	int container_change_id;
#endif
};

class MinionConstIteratorWithSlotIndex
{
	friend class Minions;

public:
	typedef MinionIterator::container_type container_type;
	typedef container_type::const_iterator const_iterator;

private: // only Minions can create instance
	MinionConstIteratorWithSlotIndex(Minions const& minions, const_iterator it_begin, SlotIndex slot_idx_begin);

public:
	SlotIndex GetSlotIdx() const;

	const_iterator::pointer operator->();
	const_iterator::reference & operator*();

	void GoToNext();
	bool IsEnd() const;

private:
	void CheckChangeId() const;

private:
	Minions const& minions;
	const_iterator it;
	SlotIndex slot_idx;

#ifdef DEBUG
	int container_change_id;
#endif
};

} // BoardObjects
} // GameEngine
