#pragma once

#include <list>
#include "minion.h"

namespace GameEngine {
	namespace BoardObjects {

		class Minions
		{
			friend std::hash<Minions>;

		public:
			typedef std::list<Minion> container_type;
			typedef container_type::iterator iterator;
			typedef container_type::const_iterator const_iterator;

		public:
			class IteratorWithIndex
			{
			public:
				IteratorWithIndex(SlotIndex slot_idx_begin, container_type::iterator it, container_type &container)
					: slot_idx(slot_idx_begin), it(it), container(&container) { }

				bool IsEnd() const { return this->it == container->end(); }

				void GoToNext() { this->it++; SlotIndexHelper::Increase(this->slot_idx); }
				void EraseAndGoToNext() {
					this->it = container->erase(this->it);
				}

				container_type::iterator::pointer operator->() { return this->it.operator->(); }

				SlotIndex GetSlotIdx() const { return this->slot_idx; }

			private:
				SlotIndex slot_idx;
				container_type::iterator it;
				container_type * const container;
			};

			class ConstIteratorWithIndex
			{
			public:
				ConstIteratorWithIndex(SlotIndex slot_idx_begin, container_type::const_iterator it, container_type const& container)
					: slot_idx(slot_idx_begin), it(it), container(&container) { }

				bool IsEnd() const { return this->it == container->end(); }

				void GoToNext() { this->it++; SlotIndexHelper::Increase(this->slot_idx); }

				container_type::const_iterator::pointer operator->() { return this->it.operator->(); }

				SlotIndex GetSlotIdx() const { return this->slot_idx; }

			private:
				SlotIndex slot_idx;
				container_type::const_iterator it;
				container_type const* const container;
			};

		public:
			bool operator==(Minions const& rhs) const { return this->minions == rhs.minions; }
			bool operator!=(Minions const& rhs) const { return !(*this == rhs); }

		public: // getters
			int GetMinionCount() const { return (int)this->minions.size(); }
			bool IsFull() const { return this->GetMinionCount() >= max_minions; }
			ConstIteratorWithIndex GetIteratorWithIndex(SlotIndex start_slot) const { return ConstIteratorWithIndex(start_slot, this->minions.begin(), this->minions); }
			IteratorWithIndex GetIteratorWithIndex(SlotIndex start_slot) { return IteratorWithIndex(start_slot, this->minions.begin(), this->minions); }

			const_iterator GetIterator(int idx) const {
				auto it = this->minions.cbegin();
				for (; idx > 0; --idx) ++it;
				return it;
			}

			iterator GetIterator(int idx) {
				auto it = this->minions.begin();
				for (; idx > 0; --idx) ++it;
				return it;
			}

		public: // add
			Minion & AddMinion(int idx, Minion const& minion) {
				iterator it;
				if (idx == this->minions.size()) it = this->minions.end();
				else it = this->GetIterator(idx);

				return *this->minions.insert(it, minion); 
			}

		public: // hooks
			void TurnStart(bool owner_turn) {
				for (auto & minion : this->minions) minion.TurnStart(owner_turn);
			}
			void TurnEnd(bool owner_turn) {
				for (auto & minion : this->minions) minion.TurnEnd(owner_turn);
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

			container_type minions;
		};

	}
}

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