#pragma once

#include <atomic>
#include <shared_mutex>

namespace Utils
{
	class SpinLock
	{
	public:
		SpinLock() : flag_() {}

		void lock() {
			while (flag_.test_and_set(std::memory_order_acquire)) {
				; // spin
			}
		}

		void unlock() {
			flag_.clear(std::memory_order_release);
		}

	private:
		std::atomic_flag flag_;
	};

	class SharedSpinLock
	{
	public:
		SharedSpinLock() : lock_(), writer_(false), readers_(0) {}

		void lock() {
			while (true) {
				lock_.lock();
				if (!writer_ && readers_ == 0) break;
				lock_.unlock();
			}
			writer_ = true;
			lock_.unlock();
		}

		void unlock() {
			lock_.lock();
			writer_ = false;
			lock_.unlock();
		}

		void lock_shared() {
			while (true) {
				lock_.lock();
				if (!writer_) break;
				lock_.unlock();
			}
			++readers_;
			lock_.unlock();
		}

		void unlock_shared() {
			lock_.lock();
			--readers_;
			lock_.unlock();
		}

	private:
		SpinLock lock_;
		bool writer_;
		int readers_;
	};
}