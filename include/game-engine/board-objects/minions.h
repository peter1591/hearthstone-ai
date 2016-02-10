#pragma once

#include <iostream>
#include <list>
#include "minion.h"
#include "minions-iterator.h"

namespace GameEngine {
namespace BoardObjects {

class Minions
{
	friend std::hash<Minions>;
	friend class MinionsIteratorWithIndex;
	friend class MinionsConstIteratorWithIndex;

public:
	typedef std::list<MinionsItem> container_type;
	typedef container_type::iterator iterator;
	typedef container_type::const_iterator const_iterator;

public:
	Minions() : pending_removal_count(0) {}

	Minions(Minions const& rhs) = delete;
	Minions & operator=(Minions const& rhs) = delete;
	Minions & CloneFrom(Minions const& rhs);

	Minions(Minions && rhs) { *this = std::move(rhs); }
	Minions & operator=(Minions && rhs);

	// for copy
	void FillMinionPointersMap(Minions const& rhs, std::map<Minion const*, Minion const*> & minion_ptr_map) const
	{
		if (this->minions.size() != rhs.minions.size()) throw std::runtime_error("minions should equal after copying");

		auto it_lhs = this->minions.begin();
		auto it_rhs = rhs.minions.begin();

		while (true)
		{
			if (it_lhs == this->minions.end()) break;
			if (it_rhs == rhs.minions.end()) break;

			minion_ptr_map[&it_lhs->Get()] = &it_rhs->Get();

			it_lhs++;
			it_rhs++;
		}
	}

	bool operator==(Minions const& rhs) const { return this->minions == rhs.minions; }
	bool operator!=(Minions const& rhs) const { return !(*this == rhs); }

public: // getters
	int GetMinionCount() const { return (int)this->minions.size() - this->pending_removal_count; }
	bool IsFull() const { return this->GetMinionCount() >= max_minions; }

	MinionsConstIteratorWithIndex GetIteratorWithIndex(SlotIndex start_slot) const { 
		return MinionsConstIteratorWithIndex(start_slot, this->minions.begin(), *this);
	}
	MinionsIteratorWithIndex GetIteratorWithIndex(SlotIndex start_slot) { 
		return MinionsIteratorWithIndex(start_slot, this->minions.begin(), *this);
	}

	Minion const& Get(int idx) const {
		auto it = this->minions.cbegin();
		for (; idx > 0; --idx) ++it;
		return it->Get();
	}

	Minion & Get(int idx) {
		auto it = this->minions.begin();
		for (; idx > 0; --idx) ++it;
		return it->Get();
	}

public: // hooks
	void TurnStart(bool owner_turn) {
		for (auto & minion : this->minions) minion.Get().TurnStart(owner_turn);
	}
	void TurnEnd(bool owner_turn) {
		for (auto & minion : this->minions) minion.Get().TurnEnd(owner_turn);
	}

public: // debug
	void DebugPrint() const {
		for (const auto &minion : this->minions) {
			std::cout << "\t" << minion.Get().GetDebugString() << std::endl;
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
		minion.Get().CheckCanBeSafelyCloned();
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