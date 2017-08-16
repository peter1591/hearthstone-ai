#pragma once

#include <vector>

namespace Utils
{
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
				container_[idx] = ItemType();
			}

			if (size > fill_to) {
				container_.resize(size);
			}
			size_ = size;
		}

		template <class Arg>
		void push_back(Arg&& arg) {
			if (size_ < container_.size()) {
				container_[size_] = arg;
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
