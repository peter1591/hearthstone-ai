#pragma once

#include <list>
#include <functional>

#include "game-engine/managed-list/list-item.h"

namespace GameEngine
{
	// Note:
	// The iterator is missing by design.
	// This container makes sure all the iterators instances (i.e., the ManagedItem) are valid
	//      until the container vanishes.
	// If a iterator class is introduced, we cannot guarantee the iterator's lifetime

	template <typename ItemType>
	class ManagedList
	{
	public:
		using ManagedItem = ManagedListItem<ItemType>;
		using UnderlyingContainer = typename ManagedItem::UnderlyingContainer;

		ManagedList() {}
		ManagedList(ManagedList<ItemType> const& rhs) = delete;
		ManagedList(ManagedList<ItemType> && rhs) = delete;

		bool operator==(ManagedList const& rhs) const { return this->items == rhs.items; }
		bool operator!=(ManagedList const& rhs) const { return this->items != rhs.items; }

		typedef std::function<bool(ItemType const&, ItemType const&)> Comparator;
		bool EqualsTo(ManagedList const& rhs, Comparator const& comparator) const;

		bool Empty() const { return this->items.empty(); }

		ManagedItem PushFront(ItemType const& item) { return ManagedItem(*this, this->items.insert(this->items.begin(), item)); }
		ManagedItem PushFront(ItemType && item) { return ManagedItem(*this, this->items.insert(this->items.begin(), std::move(item))); }

		ManagedItem PushBack(ItemType const& item) { return ManagedItem(*this, this->items.insert(this->items.end(), item)); }
		ManagedItem PushBack(ItemType && item) { return ManagedItem(*this, this->items.insert(this->items.end(), std::move(item))); }

		void ForEach(std::function<void(ItemType &)> func) {
			for (auto & item : this->items) func(item);
		}
		void ForEach(std::function<void(ItemType const&)> func) const {
			for (auto const& item : this->items) func(item);
		}

		// Note: Invalidate all ManagedItem which are removed
		void RemoveIf(std::function<bool(ManagedItem)> func) {
			for (auto it = this->items.begin(); it != this->items.end();) {
				if (func(ManagedItem(*this, it))) {
					it = this->items.erase(it);
				}
				else {
					++it;
				}
			}
		}

	public: // only used by ManagedListIterator
		void Erase(typename UnderlyingContainer::iterator it) { this->items.erase(it); }

	private:
		UnderlyingContainer items;
	};

	template<typename ItemType>
	inline bool ManagedList<ItemType>::EqualsTo(ManagedList<ItemType> const & rhs, Comparator const & comparator) const
	{
		if (this->items.size() != rhs.items.size()) return false;

		auto it_lhs = this->items.begin();
		auto it_rhs = rhs.items.begin();

		while (true)
		{
			if (it_lhs == this->items.end()) break;
#ifdef DEBUG
			if (it_rhs == rhs.items.end()) throw std::runtime_error("both iterators should reach end at the same time");
#endif

			if (!comparator(*it_lhs, *it_rhs)) return false;

			it_lhs++;
			it_rhs++;
		}

		// both iterators should reach end since they have the same size
		return true;
	}

} // namespace GameEngine

#include "game-engine/managed-list/list-item-impl.h"