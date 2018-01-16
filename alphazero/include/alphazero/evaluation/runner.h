#pragma once

#include "alphazero/evaluation/evaluator.h"

#include <vector>

namespace alphazero
{
	namespace evaluation
	{
		class Runner
		{
		public:
			Runner(ILogger & logger) :
				logger_(logger),
				run_options_(),
				evaluators_()
			{}

			void Initialize(int threads) {
				for (int i = 0; i < threads; ++i) {
					evaluators_.emplace_back();
				}
			}

			void BeforeRun(
				int milliseconds,
				std::vector<detail::ThreadRunner*> const& threads,
				neural_net::NeuralNet const& baseline,
				neural_net::NeuralNet const& competitor)
			{
				assert(threads.size() <= evaluators_.size());

				auto start = std::chrono::steady_clock::now();
				auto until = start + std::chrono::milliseconds(milliseconds);

				for (size_t i = 0; i < threads.size(); ++i) {
					evaluators_[i].BeforeRun(baseline, competitor);
				}

				for (size_t i = 0; i < threads.size(); ++i) {
					auto & evaluator = evaluators_[i];
					threads[i]->AsyncRunUntil(until, [&evaluator, this]() {
						evaluator.RunOnce(run_options_);
					});
				}
			}

			// TODO: should not require argument
			evaluation::CompetitionResult AfterRun(std::vector<detail::ThreadRunner*> const& threads) {
				evaluation::CompetitionResult result;
				for (size_t i = 0; i < threads.size(); ++i) {
					result += evaluators_[i].AfterRun();
				}

				return result;
			}

		private:
			ILogger & logger_;
			RunOptions run_options_;
			std::vector<evaluation::Evaluator> evaluators_;
		};
	}
}