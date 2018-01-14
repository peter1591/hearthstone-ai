#pragma once

#include <assert.h>
#include <vector>

namespace alphazero
{
	namespace shared_data
	{
		template <class T> class BatchItems
		{
		public:
			BatchItems(size_t capacity) : capacity_(capacity), items_() {
				items_.reserve(capacity);
			}

			size_t GetCapacity() const { return capacity_; }
			bool IsFull() const { return items_.size() == capacity_; }

			void PushBack(T const& item) {
				assert(!IsFull());
				items_.push_back(item);
			}

			void Clear() {
				items_.clear();
			}

			T & Get(size_t idx) {
				assert(idx < items_.size());
				return items_[idx];
			}
			T const& Get(size_t idx) const {
				assert(idx < items_.size());
				return items_[idx];
			}

		private:
			size_t capacity_;
			std::vector<T> items_;
		};
	}
}