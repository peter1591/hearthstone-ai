#pragma once

#include <iostream>
#include <list>
#include "minion-data.h"
#include "minion-iterator.h"
#include "minion.h"

namespace GameEngine {

class Player;

class Minions
{
	friend std::hash<Minions>;

public:
	typedef MinionIterator::container_type container_type;
	typedef container_type::iterator iterator;
	typedef container_type::const_iterator const_iterator;

public:
	Minions(Player & player) : player(player), pending_removal_count(0)
	{
#ifdef DEBUG
		this->change_id = 0;
#endif
	}

	Minions(Player & player, Minions const& rhs) :
		player(player), pending_removal_count(rhs.pending_removal_count)
	{
		for (auto const& minion : rhs.minions) {
			this->minions.push_back(GameEngine::Minion(*this, minion));
		}
	}
	Minions(Player & Player, Minions && rhs) : player(player) { *this = std::move(rhs); }

	Minions & operator=(Minions const& rhs) = delete;
	Minions & operator=(Minions && rhs);

	bool operator==(Minions const& rhs) const { return this->minions == rhs.minions; }
	bool operator!=(Minions const& rhs) const { return !(*this == rhs); }

	void Destroy();

	container_type::iterator begin() { return this->minions.begin(); }
	container_type::const_iterator begin() const { return this->minions.begin(); }
	container_type::iterator end() { return this->minions.end(); }
	container_type::const_iterator end() const { return this->minions.end(); }

	Board & GetBoard() const;

public: // getters
	int GetMinionCount() const { return (int)this->minions.size() - this->pending_removal_count; }
	bool IsFull() const { return this->GetMinionCount() >= max_minions; }

	MinionConstIteratorWithSlotIndex GetMinionsIteratorWithIndexAtBegin(SlotIndex start_idx) const {
		return MinionConstIteratorWithSlotIndex(*this, this->minions.begin(), start_idx);
	}

	MinionIterator GetIterator(int minion_idx) {
		return MinionIterator(this->GetBoard(), *this, this->GetRawIterator(minion_idx));
	}

	MinionIterator GetIteratorForSpecificMinion(Minion const& minion) {
		return MinionIterator(this->GetBoard(), *this, this->GetRawIteratorForSpecificMinion(minion));
	}

	Minion & GetMinion(int minion_idx) {
		auto it = this->GetRawIterator(minion_idx);
		if (it == this->minions.end()) {
			throw std::out_of_range("minion idx out of range");
		}
		return *it;
	}

public: // modifiers
	MinionIterator InsertBefore(MinionIterator const& it, MinionData && minion);
	void MarkPendingRemoval(MinionIterator const& it);
	void MarkPendingRemoval(Minion & minion);
	void EraseAndGoToNext(MinionIterator & it);

public: // hooks
	void TurnStart(bool owner_turn) {
		for (auto it = this->minions.begin(); it != this->minions.end(); ++it) {
			it->TurnStart(owner_turn);
		}
	}
	void TurnEnd( bool owner_turn) {
		for (auto it = this->minions.begin(); it != this->minions.end(); ++it) {
			it->TurnEnd(owner_turn);
		}
	}
	void HookAfterMinionAdded(Minion & added_minion) {
		for (auto it = this->minions.begin(); it != this->minions.end(); ++it) {
			it->HookAfterMinionAdded(added_minion);
		}
	}
	void HookAfterMinionDamaged(Minion & minion, int damage) {
		for (auto it = this->minions.begin(); it != this->minions.end(); ++it) {
			it->HookAfterMinionDamaged(minion, damage);
		}
	}

public: // debug
	void DebugPrint() const {
		for (const auto &minion : this->minions) {
			std::cout << "\t" << minion.GetMinion().GetDebugString() << std::endl;
		}
	}

private:
	container_type::iterator GetRawIterator(int minion_idx) {
		auto it = this->minions.begin();
		for (; minion_idx > 0; --minion_idx) {
			if (it == this->minions.end()) break;
			++it;
		}
		return it;
	}

	container_type::iterator GetRawIteratorForSpecificMinion(Minion const& minion) {
		for (auto it = this->minions.begin(); it != this->minions.end(); ++it) {
			if (&(*it) == &minion) return it;
		}
		return this->minions.end();
	}

private:
	static constexpr int max_minions = 7;

	Player & player;

	int pending_removal_count;
	container_type minions;

public:
#ifdef DEBUG
	int change_id;
#endif
};

} // GameEngine

namespace std {
	template <> struct hash<GameEngine::Minions> {
		typedef GameEngine::Minions argument_type;
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

inline GameEngine::Minions & GameEngine::Minions::operator=(Minions && rhs)
{
	this->pending_removal_count = std::move(rhs.pending_removal_count);

	this->minions.clear();
	for (auto & minion : rhs.minions) {
		this->minions.push_back(Minion(*this, std::move(minion)));
	}

	return *this;
}
