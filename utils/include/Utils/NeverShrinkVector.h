#pragma once

#include <vector>

namespace Utils
{
	// Differs with std::optional. The ctor/dtor will not be invoked
	// during an set/unset operation.
	template <class ItemType>
	class PersistentOptionalItem {
	public:
		PersistentOptionalItem() : exist_(false), item_() {}

		PersistentOptionalItem(ItemType const& rhs) :
			exist_(true), item_(rhs)
		{}

		PersistentOptionalItem(ItemType && rhs) :
			exist_(true), item_(std::move(rhs))
		{}

		void Set(ItemType const& item) {
			exist_ = true;
			item_ = item;
		}

		void SetWithBase(ItemType const& item) {
			exist_ = true;
			item_.FillWithBase(item);
		}

		void UnSet() {
			exist_ = false;
		}

		bool HasSet() const { return exist_; }

		ItemType & Get() {
			assert(exist_);
			return item_;
		}

		ItemType const& Get() const {
			assert(exist_);
			return item_;
		}

	private:
		bool exist_;
		ItemType item_;
	};

	template <class TestType>
	struct IsPersistentOptionalItem : std::false_type {};

	template <class ItemType>
	struct IsPersistentOptionalItem<PersistentOptionalItem<ItemType>> : std::true_type {};

	template <class ItemType>
	class NeverShrinkVector {
	public:
		NeverShrinkVector() : size_(), container_() {}
		NeverShrinkVector(size_t size) : size_(size), container_(size) {}

		auto begin() {
			return container_.begin();
		}
		auto end() {
			return container_.begin() + size_;
		}

		auto const& operator[](size_t idx) const {
			assert(idx < size_);
			return container_[idx];
		}
		auto & operator[](size_t idx) {
			assert(idx < size_);
			return container_[idx];
		}

		void clear() { size_ = 0; }
		size_t size() const { return size_; }

		void resize(size_t size) {
			size_t fill_to = std::min(size, container_.size());
			for (size_t idx = size_; idx < fill_to; ++idx) {
				if constexpr (IsPersistentOptionalItem<ItemType>::value) {
					container_[idx].UnSet();
				}
				else {
					container_[idx] = ItemType();
				}
			}

			if (size > fill_to) {
				container_.resize(size);
			}
			size_ = size;
		}

		template <class Arg>
		void push_back(Arg&& arg) {
			if (size_ < container_.size()) {
				if constexpr (IsPersistentOptionalItem<ItemType>::value) {
					container_[size_].Set(std::forward<Arg>(arg));
				}
				else {
					container_[size_] = arg;
				}
				++size_;
			}
			else {
				assert(size_ == container_.size());
				container_.push_back(std::forward<Arg>(arg));
				++size_;
			}
		}

	private:
		size_t size_;
		std::vector<ItemType> container_;
	};
}
