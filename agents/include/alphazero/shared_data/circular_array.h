#pragma once

#include <assert.h>
#include <atomic>
#include <mutex>
#include <vector>

namespace alphazero
{
	namespace shared_data
	{
		// A lockless circular array.
		// Thread safety: Yes, with two restrictions:
		//   1. capacity must be a power of 2, since we use lower bits to be the index
		//   2. Item type T is exposed as it is. It should be thread-safe.
		template <class T>
		class CircularArray
		{
		public:
			CircularArray() :
				head_(0),
				index_mask_(0),
				items_()
			{
			}

			void Initialize(size_t capacity_power_two) {
				// make sure index type is large enough
				assert(8 * sizeof(head_) > capacity_power_two);

				size_t capacity = (size_t)1 << capacity_power_two;
				index_mask_ = capacity - 1;
				head_ = 0;
				items_.resize(capacity);
			}

			size_t GetCapacity() const {
				return items_.size();
			}

			// Thread-safe if number of concurrent callers less than array size
			T & AllocateNext() {
				size_t idx = head_.fetch_add(1);
				auto & ret = items_[GetIndex(idx)];
				return ret;
			}

			T const& Get(size_t idx) const {
				idx += head_.load();
				return items_[GetIndex(idx)];
			}

			// don't need to fetch head index, since we're fetching a random item
			T const& RandomGet(size_t idx) const {
				return items_[GetIndex(idx)];
			}

		private:
			size_t GetIndex(size_t v) const {
				return v & index_mask_;
			}

		private:
			std::atomic<size_t> head_;
			size_t index_mask_;
			std::vector<T> items_;
		};
	}
}