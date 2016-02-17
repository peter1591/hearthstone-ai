#pragma once

#include <iostream>
#include <list>
#include "minion.h"
#include "minion-iterator.h"
#include "minion-manipulator.h"

namespace GameEngine {
namespace BoardObjects {
namespace Impl {
	class MinionIteratorHelper;
}

class Minions
{
	friend std::hash<Minions>;

	// Iterators for minions
	friend class Impl::MinionIteratorHelper;

public:
	typedef MinionIterator::container_type container_type;
	typedef container_type::iterator iterator;
	typedef container_type::const_iterator const_iterator;

public:
	Minions(Board & board) : board(board), pending_removal_count(0) {}

	Minions(Minions const& rhs) = delete;
	Minions & operator=(Minions const& rhs);

	Minions(Board & board, Minions && rhs) : board(board) { *this = std::move(rhs); }
	Minions & operator=(Minions && rhs);

	bool operator==(Minions const& rhs) const { return this->minions == rhs.minions; }
	bool operator!=(Minions const& rhs) const { return !(*this == rhs); }

	container_type::iterator begin() { return this->minions.begin(); }
	container_type::const_iterator begin() const { return this->minions.begin(); }
	container_type::iterator end() { return this->minions.end(); }
	container_type::const_iterator end() const { return this->minions.end(); }

public: // getters
	int GetMinionCount() const { return (int)this->minions.size() - this->pending_removal_count; }
	bool IsFull() const { return this->GetMinionCount() >= max_minions; }

	Minion& GetMinion(int minion_idx) {
		auto it = this->minions.begin();
		for (; minion_idx > 0; --minion_idx) {
			++it;
			if (it == this->minions.end()) {
				throw std::out_of_range("minion idx out of range");
			}
		}
		return *it;
	}

	MinionIterator GetIterator(int minion_idx) {
		return MinionIterator(this->board, *this, this->GetRawIterator(minion_idx));
	}

	MinionIterator GetIterator(Minion const& minion) {
		return MinionIterator(this->board, *this, this->GetRawIterator(&minion));
	}

	MinionManipulator GetManipulator(int minion_idx) {
		auto it = this->GetRawIterator(minion_idx);
		if (it == this->minions.end()) {
			throw std::out_of_range("minion idx out of range");
		}
		return MinionManipulator(this->board, *this, *it);
	}

	MinionManipulator GetManipulator(Minion const* minion) 
	{
		auto it = this->GetRawIterator(minion);
		if (it == this->minions.end()) throw std::runtime_error("cannot find minion");
		return MinionManipulator(this->board, *this, *it);
	}

public: // modifiers
	MinionManipulator InsertBefore(MinionIterator const& it, Minion && minion);
	void MarkPendingRemoval(MinionIterator const& it);
	void EraseAndGoToNext(MinionIterator & it);

public: // hooks
	void TurnStart(bool owner_turn) {
		for (auto it = this->minions.begin(); it != this->minions.end(); ++it) {
			MinionManipulator(this->board, *this, *it).TurnStart(owner_turn);
		}
	}
	void TurnEnd( bool owner_turn) {
		for (auto it = this->minions.begin(); it != this->minions.end(); ++it) {
			MinionManipulator(this->board, *this, *it).TurnEnd(owner_turn);
		}
	}
	void HookAfterMinionAdded(MinionManipulator & added_minion) {
		for (auto it = this->minions.begin(); it != this->minions.end(); ++it) {
			MinionManipulator(this->board, *this, *it).HookAfterMinionAdded(added_minion);
		}
	}

public: // debug
	void DebugPrint() const {
		for (const auto &minion : this->minions) {
			std::cout << "\t" << minion.GetDebugString() << std::endl;
		}
	}

private:
	container_type::iterator GetRawIterator(int minion_idx) {
		auto it = this->minions.begin();
		for (; minion_idx > 0; --minion_idx) {
			++it;
			if (it == this->minions.end()) break;
		}
		return it;
	}

	container_type::iterator GetRawIterator(Minion const* minion)
	{
		// Note: A linear search alrogithm
		for (auto it = this->minions.begin(); it != this->minions.end(); ++it) {
			if (&(*it) == minion) return it;
		}
		return this->minions.end();
	}

private:
	static constexpr int max_minions = 7;

	Board & board;

	int pending_removal_count;
	container_type minions;
};

} // BoardObjects
} // GameEngine

namespace std {
	template <> struct hash<GameEngine::BoardObjects::Minions> {
		typedef GameEngine::BoardObjects::Minions argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			for (auto const& minion : s.minions) {
				GameEngine::hash_combine(result, minion);
			}

			return result;
		}
	};
}

inline GameEngine::BoardObjects::Minions & GameEngine::BoardObjects::Minions::operator=(Minions const & rhs)
{
	// The MCTS should only clone board from the initialized state
	// If caller need to clone the initialized state (which has some enchantment placed on heap)
	// we need to ask the caller to re-initialized the board as he did at the first time
#ifdef DEBUG
	for (auto const& minion : rhs.minions)
	{
		minion.CheckCanBeSafelyCloned();
	}
#endif

	this->pending_removal_count = rhs.pending_removal_count;
	this->minions = rhs.minions;

	return *this;
}

inline GameEngine::BoardObjects::Minions & GameEngine::BoardObjects::Minions::operator=(Minions && rhs)
{
	this->pending_removal_count = std::move(rhs.pending_removal_count);
	this->minions = std::move(rhs.minions);

	return *this;
}

#include "minion-iterator-impl.h"
#include "enchantments-impl.h"
