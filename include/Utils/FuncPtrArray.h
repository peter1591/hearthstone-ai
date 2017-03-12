#pragma once

#include <assert.h>
#include <array>
#include <utility>
#include <type_traits>

namespace Utils
{
	template <typename FuncPtr, size_t Size>
	class FuncPtrArray
	{
	public:
		FuncPtrArray() : size_(0) {}

		void operator+=(FuncPtr item) {
			assert(size_ < Size);
			items_[size_] = item;
			++size_;
		}

		template <typename... Args>
		void operator()(Args&&... args) const {
			for (size_t i = 0; i < size_; ++i) {
				(*items_[i])(std::forward<Args>(args)...);
			}
		}

	private:
		std::array<FuncPtr, Size> items_;
		size_t size_;
	};

	template <typename FuncPtr>
	class FuncPtrArray<FuncPtr, 1>
	{
	public:
		FuncPtrArray() : item_(nullptr) {}

		void operator+=(FuncPtr item) {
			assert(item_ == nullptr);
			item_ = item;
		}

		template <typename... Args>
		void operator()(Args&&... args) const {
			if (!item_) return;
			(*item_)(std::forward<Args>(args)...);
		}

	private:
		FuncPtr item_;
	};
}