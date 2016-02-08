#pragma once

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

#include "minions-iterator-impl.h"