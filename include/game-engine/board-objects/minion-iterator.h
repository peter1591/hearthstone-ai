#pragma once

#include <list>
#include "minion-manipulator.h"

namespace GameEngine {
namespace BoardObjects {

class Minions;

class MinionIterator
{
	friend class Minions;

public:
	typedef std::list<MinionManipulator> container_type;

private: // only Minions can create instance
	MinionIterator(Board & board, Minions & minions, container_type::iterator it);

public:
	Board & GetBoard() const;
	Minions & GetMinions() const;
	container_type::iterator GetIterator() const;
	Minion const& GetMinion() const;

	bool IsPlayerSide() const;
	bool IsOpponentSide() const;

	void GoToNext();
	bool IsEnd() const;

	MinionManipulator & ConvertToManipulator() { 
		if (this->IsEnd()) throw std::runtime_error("minion vanished");
		return *it;
	}

private:
	void CheckChangeId() const;

private:
	Board & board;
	Minions & minions;
	container_type::iterator it;

#ifdef DEBUG
	int container_change_id;
#endif
};

class MinionConstIteratorWithSlotIndex
{
	friend class Minions;

public:
	typedef MinionIterator::container_type container_type;

private: // only Minions can create instance
	MinionConstIteratorWithSlotIndex(Minions const& minions, container_type::const_iterator it_begin, SlotIndex slot_idx_begin);

public:
	Minion const& GetMinion() const;
	SlotIndex GetSlotIdx() const;

	void GoToNext();
	bool IsEnd() const;

private:
	void CheckChangeId() const;

private:
	Minions const& minions;
	container_type::const_iterator it;
	SlotIndex slot_idx;

#ifdef DEBUG
	int container_change_id;
#endif
};

} // BoardObjects
} // GameEngine
