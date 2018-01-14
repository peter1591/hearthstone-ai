#pragma once

#include <assert.h>
#include <functional>
#include <thread>
#include <memory>
#include <condition_variable>

#include "neural_net/optimizer.h"

namespace alphazero
{
	namespace detail {
		// Runner controls what should be done on a specific thread
		class ThreadRunner
		{
		public:
			ThreadRunner() : thread_(), task_() {}

			ThreadRunner(ThreadRunner const&) = delete;
			ThreadRunner & operator=(ThreadRunner const&) = delete;

			// Thread safety: should be called in main thread
			void Initialize() {
				assert(!thread_);
				thread_.reset(new std::thread(ThreadMain, this));
			}

			// Thread safety: should be called in main thread
			void Release() {
				if (thread_) thread_->join();
			}

			// Thread safety: should be called in main thread
			void AsyncRun(std::function<void()> task) {
				std::lock_guard<std::mutex> guard(task_mutex_);
				assert(task);
				if (task_) throw std::runtime_error("You should wait for the previous work done.");
				task_ = std::move(task);
				assert(task_);
				task_cv_.notify_one();
			}

			// Thread safety: should be called in main thread
			void Wait() {
				std::unique_lock<std::mutex> lock(task_mutex_);
				while (task_) task_cv_.wait(lock);
			}

		private:
			static void ThreadMain(ThreadRunner * runner) {
				runner->MainLoop();
			}

			void MainLoop() {
				while (true) {
					std::unique_lock<std::mutex> lock(task_mutex_);
					while (!task_) task_cv_.wait(lock);

					task_();

					task_ = std::function<void()>(); // indicate done
					task_cv_.notify_one();
				}
			}

		private:
			std::unique_ptr<std::thread> thread_; // TODO: remove unique_ptr?
			std::function<void()> task_;
			std::mutex task_mutex_;
			std::condition_variable task_cv_;
		};
	}
}