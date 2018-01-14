#pragma once

#include <assert.h>
#include <chrono>
#include <functional>
#include <thread>
#include <memory>
#include <condition_variable>

#include "neural_net/optimizer.h"
#include "evaluation/evaluator.h"
#include "self_play/self_player.h"
#include "detail/thread_runner.h"

namespace alphazero
{
	// Runner controls what should be done on a specific thread
	class Runner
	{
	public:
		Runner() : running_threads_(), optimizer_options_() {}

		void Initialize() {
			// TODO: adjust at runtime?
			// TODO: or, dynamically adjust parameters, so the responsive time meets requirements (say, 100ms)?
			optimizer_options_.epochs_per_run = 1000;
			evaluation_options_.epochs_per_run = 1000;
			self_player_options_.MCTS_iterations_per_run = 1000;
		}

		// Thread safety: Can only be invoked at main thread
		template <class... Args>
		void AsyncRun(detail::ThreadRunner & thread, int milliseconds, neural_net::Optimizer & optimizer, Args&&... args) {
			auto start = std::chrono::steady_clock::now();

			thread.AsyncRun([start, milliseconds, &optimizer, &args..., this]() -> void {
				while (true) {
					optimizer.RunOnce(optimizer_options_, std::forward<Args>(args)...);

					auto now = std::chrono::steady_clock::now();
					auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
					if (duration > milliseconds) {
						return; // time's up!
					}
				}
			});

			running_threads_.push_back(&thread);
		}

		// Thread safety: Can only be invoked at main thread
		template <class... Args>
		void AsyncRun(detail::ThreadRunner & thread, int milliseconds, evaluation::Evaluator & evaluator, Args&&... args) {
			auto start = std::chrono::steady_clock::now();

			thread.AsyncRun([&]() -> void {
				while (true) {
					evaluator.RunOnce(std::forward<Args>(args)..., evaluation_options_);

					auto now = std::chrono::steady_clock::now();
					if (std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count() > milliseconds) {
						return; // time's up!
					}
				}
			});

			running_threads_.push_back(&thread);
		}

		// Thread safety: Can only be invoked at main thread
		template <class... Args>
		void AsyncRun(detail::ThreadRunner & thread, int milliseconds, self_play::SelfPlayer & self_play, Args&&... args) {
			auto start = std::chrono::steady_clock::now();

			thread.AsyncRun([&]() -> void {
				while (true) {
					self_play.RunOnce(std::forward<Args>(args)..., self_player_options_);

					auto now = std::chrono::steady_clock::now();
					if (std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count() > milliseconds) {
						return; // time's up!
					}
				}
			});

			running_threads_.push_back(&thread);
		}

		// Thread safety: Can only be invoked at main thread
		void WaitAll() {
			for (auto thread : running_threads_) {
				thread->Wait();
			}
			running_threads_.clear();
		}

	private:
		std::vector<detail::ThreadRunner*> running_threads_;
		neural_net::OptimizerRunOptions optimizer_options_;
		evaluation::EvaluatorRunOptions evaluation_options_;
		self_play::SelfPlayerRunOptions self_player_options_;
	};
}