#pragma once

#include <iostream>
#include <list>
#include "minion.h"
#include "minion-manipulator.h"
#include "minions-iterator.h"

namespace GameEngine {
namespace BoardObjects {

class Minions
{
	friend std::hash<Minions>;
	friend class MinionConstIteratorWithSlotIndex;
	friend class MinionInserter;

public:
	typedef std::list<Minion> container_type;
	typedef container_type::iterator iterator;
	typedef container_type::const_iterator const_iterator;

public:
	Minions() : pending_removal_count(0) {}

	Minions(Minions const& rhs) = delete;
	Minions & operator=(Minions const& rhs) = delete;
	Minions & CloneFrom(Minions const& rhs);

	Minions(Minions && rhs) { *this = std::move(rhs); }
	Minions & operator=(Minions && rhs);

	bool operator==(Minions const& rhs) const { return this->minions == rhs.minions; }
	bool operator!=(Minions const& rhs) const { return !(*this == rhs); }

public: // getters
	int GetMinionCount() const { return (int)this->minions.size() - this->pending_removal_count; }
	bool IsFull() const { return this->GetMinionCount() >= max_minions; }

	MinionConstIteratorWithSlotIndex GetIteratorWithSlotIndex(SlotIndex start_slot) const { 
		return MinionConstIteratorWithSlotIndex(start_slot, this->minions.begin(), *this);
	}

	MinionInserter GetInserter(GameEngine::Board & board, int minion_idx) {
		auto it = this->minions.begin();
		for (; minion_idx > 0; --minion_idx) {
			++it;
			if (it == this->minions.end()) break;
		}
		return MinionInserter(board, *this, it);
	}

	MinionManipulator GetManipulator(GameEngine::Board & board, int minion_idx) {
		auto inserter = this->GetInserter(board, minion_idx);
		if (inserter.IsEnd()) throw std::runtime_error("invalid argument");
		return inserter.ConverToManipulator();
	}

	MinionManipulator GetManipulator(GameEngine::Board & board, Minion * minion) {
#ifdef DEBUG
		bool found = false;
		for (auto const& item : this->minions) {
			if (&item == minion) {
				found = true;
				break;
			}
		}
		if (!found) throw std::runtime_error("cannot find minion");
#endif

		return MinionManipulator(board, *this, *minion);
	}

	Minion const& Get(int idx) const {
		auto it = this->minions.cbegin();
		for (; idx > 0; --idx) ++it;
		return *it;
	}

	Minion & Get(int idx) {
		auto it = this->minions.begin();
		for (; idx > 0; --idx) {
			++it;
			if (it == this->minions.end()) break;
		}
		return *it;
	}

	container_type::iterator GetIterator(Minion * const minion)
	{
		for (auto it = this->minions.begin(); it != this->minions.end(); ++it) {
			if (&(*it) == minion) return it;
		}
		return this->minions.end();
	}

public: // debug
	void DebugPrint() const {
		for (const auto &minion : this->minions) {
			std::cout << "\t" << minion.GetDebugString() << std::endl;
		}
	}

private:
	static constexpr int max_minions = 7;

private:
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

inline GameEngine::BoardObjects::Minions & GameEngine::BoardObjects::Minions::CloneFrom(Minions const & rhs)
{
	// The MCTS should only clone board from the initialized state
	// If caller need to clone the initialized state (which has some enchantment placed on heap)
	// we need to ask the caller to re-initialized the board as he did at the first place
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

#include "minions-iterator-impl.h"
#include "enchantments-impl.h"
