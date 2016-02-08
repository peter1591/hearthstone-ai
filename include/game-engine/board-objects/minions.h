#pragma once

#include <list>
#include "minion.h"

namespace GameEngine {
	namespace BoardObjects {

		class Minions;

		class MinionsIteratorWithIndex
		{
		public:
			typedef std::list<Minion> container_type;

		public:
			MinionsIteratorWithIndex(SlotIndex slot_idx_begin, container_type::iterator it_begin, Minions &container)
				: slot_idx(slot_idx_begin), it(it_begin), container(container) { }

			bool IsEnd() const;
			
			void GoToNext();
			void EraseAndGoToNext();
			Minion& InsertBefore(Minion && minion);

			container_type::iterator::pointer operator->() { return this->it.operator->(); }

			Minions & GetOwner() const { return this->container; }
			SlotIndex GetSlotIdx() const { return this->slot_idx; }

		private:
			SlotIndex slot_idx;
			container_type::iterator it;
			Minions & container;
		};

		class MinionsConstIteratorWithIndex
		{
		public:
			typedef std::list<Minion> container_type;

		public:
			MinionsConstIteratorWithIndex(SlotIndex slot_idx_begin, container_type::const_iterator it_begin, Minions const& container)
				: slot_idx(slot_idx_begin), it(it_begin), container(container) { }

			bool IsEnd() const;
			void GoToNext();

			container_type::const_iterator::pointer operator->() { return this->it.operator->(); }

			Minions const& GetOwner() const { return this->container; }
			SlotIndex GetSlotIdx() const { return this->slot_idx; }

		private:
			SlotIndex slot_idx;
			container_type::const_iterator it;
			Minions const& container;
		};

		class Minions
		{
			friend std::hash<Minions>;
			friend class MinionsIteratorWithIndex;
			friend class MinionsConstIteratorWithIndex;

		public:
			typedef std::list<Minion> container_type;
			typedef container_type::iterator iterator;
			typedef container_type::const_iterator const_iterator;
			
		public:
			bool operator==(Minions const& rhs) const { return this->minions == rhs.minions; }
			bool operator!=(Minions const& rhs) const { return !(*this == rhs); }

		public: // getters
			int GetMinionCount() const { return (int)this->minions.size(); }
			bool IsFull() const { return this->GetMinionCount() >= max_minions; }
			MinionsConstIteratorWithIndex GetIteratorWithIndex(SlotIndex start_slot) const { 
				return MinionsConstIteratorWithIndex(start_slot, this->minions.begin(), *this);
			}
			MinionsIteratorWithIndex GetIteratorWithIndex(SlotIndex start_slot) { 
				return MinionsIteratorWithIndex(start_slot, this->minions.begin(), *this);
			}

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

		inline bool MinionsIteratorWithIndex::IsEnd() const 
		{
			return this->it == this->container.minions.end(); 
		}

		inline void MinionsIteratorWithIndex::GoToNext() 
		{
			this->it++;
			SlotIndexHelper::Increase(this->slot_idx);
		}

		inline void MinionsIteratorWithIndex::EraseAndGoToNext()
		{
			this->it = this->container.minions.erase(this->it);
		}

		inline Minion & MinionsIteratorWithIndex::InsertBefore(Minion && minion)
		{
			auto ret = this->container.minions.insert(this->it, std::move(minion));
			SlotIndexHelper::Increase(this->slot_idx);
#ifdef DEBUG
			if (this->slot_idx == SLOT_OPPONENT_SIDE || this->slot_idx == SLOT_MAX) throw std::runtime_error("minion excess range");
#endif
			return *ret;
		}

		inline bool MinionsConstIteratorWithIndex::IsEnd() const
		{
			return this->it == this->container.minions.end();
		}

		inline void MinionsConstIteratorWithIndex::GoToNext()
		{
			this->it++; SlotIndexHelper::Increase(this->slot_idx);
		}
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