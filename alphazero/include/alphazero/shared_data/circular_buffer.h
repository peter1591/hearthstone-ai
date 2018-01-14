#pragma once

#include <assert.h>
#include <vector>

namespace alphazero
{
	namespace shared_data
	{
		// Fixed capacity.
		// Assuming it's full all the time. Items are default-constructed at first.
		// Automatically pop-up the eariliest item when Push().
		// No need to support Pop().
		// Need to support random access
		// Date type T needs to support Clear(). To reset its state to default-constructed.
		// Thread safety: No
		template <class T>
		class CircularBuffer
		{
		public:
			CircularBuffer() : head_idx_(0), items_() {}

			void Initialize(size_t capacity, T const& default_val)
			{
				head_idx_ = 0;
				items_.clear();
				items_.resize(capacity, default_val);
			}

			T & Get(size_t idx) {
				assert(idx < (int)items_.size());
				return items_[GetIndex(idx)];
			}
			T const& Get(size_t idx) const {
				assert(idx < (int)items_.size());
				return items_[GetIndex(idx)];
			}

			T & AllocateNext() {
				T & ret = items_[head_idx_];
				++head_idx_;
				if (head_idx_ == items_.size()) head_idx_ = 0;
			}

		private:
			size_t GetIndex(size_t idx) const {
				return ((size_t)head_idx_ + idx) % items_.size();
			}

		private:
			int head_idx_; // index for the next push
			std::vector<T> items_;
		};
	}
}