#pragma once

#include <assert.h>
#include <vector>

#include "alphazero/detail/thread_runner.h"

namespace alphazero
{
	namespace detail {
		class ThreadPool
		{
		public:
			ThreadPool() : threads_() {}

			ThreadPool(ThreadPool const&) = delete;
			ThreadPool & operator=(ThreadPool const&) = delete;

			void Initialize(int threads) {
				assert(threads_.empty());
				for (int i = 0; i < threads; ++i) {
					threads_.emplace_back(new ThreadRunner());
					threads_.back()->Initialize();
				}
			}

			auto & Get(size_t idx) {
				return *threads_[idx];
			}

			size_t Size() const { return threads_.size(); }

			void Release() {
				for (auto & thread : threads_) {
					thread->Wait();
				}
				threads_.clear();
			}

		private:
			std::vector<std::unique_ptr<detail::ThreadRunner>> threads_;
		};
	}
}