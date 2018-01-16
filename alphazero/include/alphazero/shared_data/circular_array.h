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
		// Limitation:
		//    capacity must be a power of 2, since we use lower bits to be the index
		// Thread safety: Yes, with two restrictions:
		//   1. #-of-allocating-items < capacity
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

			// Thread-safe if number of concurrent callers less than array size
			T & AllocateNext() {
				size_t idx = head_.fetch_add(1);
				return items_[GetIndex(idx)];
			}

			template <class Filler>
			void AllocateNextN(size_t n, Filler&& filler) {
				size_t idx = head_.fetch_add(n);
				for (size_t i = 0; i < n; ++i) {
					filler(i, items_[GetIndex(idx + i)]);
				}
			}

			T const& Get(size_t idx) const {
				idx = idx + head_.load();
				return items_[GetIndex(idx)];
			}

			// don't need to fetch head index, since we're fetching a random item
			T const& RandomGet(std::mt19937 & random) const {
				return items_[GetIndex((size_t)random())];
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