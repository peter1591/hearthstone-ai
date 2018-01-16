#pragma once

#include "alphazero/self_play/self_player.h"
#include "alphazero/self_play/options.h"
#include "alphazero/detail/thread_runner.h"

namespace alphazero
{
	namespace self_play
	{
		class Runner
		{
		public:
			Runner(ILogger & logger) :
				logger_(logger),
				run_options_(),
				players_()
			{}

			void Initialize(int threads, shared_data::TrainingData & training_data) {
				for (int i = 0; i < threads; ++i) {
					players_.emplace_back(logger_);
				}
				training_data_ = &training_data;
			}

			void BeforeRun(int milliseconds, std::vector<detail::ThreadRunner*> const& threads, neural_net::NeuralNet const& neural_net) {
				assert(threads.size() <= players_.size());

				auto start = std::chrono::steady_clock::now();
				auto until = start + std::chrono::milliseconds(milliseconds);

				for (size_t i = 0; i < threads.size(); ++i) {
					players_[i].BeforeRun(*training_data_, neural_net);
				}

				for (size_t i = 0; i < threads.size(); ++i) {
					auto & player = players_[i];
					threads[i]->AsyncRunUntil(until, [&player, this]() {
						player.RunOnce(run_options_);
					});
				}
			}

			// TODO: should not require argument
			void AfterRun(std::vector<detail::ThreadRunner*> const& threads) {
				for (size_t i = 0; i < threads.size(); ++i) {
					players_[i].AfterRun();
				}
			}

		private:
			ILogger & logger_;
			RunOptions run_options_;
			shared_data::TrainingData * training_data_;
			std::vector<self_play::SelfPlayer> players_;
		};
	}
}