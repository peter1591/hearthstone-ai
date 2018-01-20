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
				evaluators_(),
				result_(),
				running_evaluators_(0)
			{}

			Runner(Runner const&) = delete;
			Runner & operator=(Runner const&) = delete;

			void Initialize(int threads) {
				for (int i = 0; i < threads; ++i) {
					evaluators_.emplace_back();
				}
			}

			void BeforeRun(
				RunOptions const& run_options,
				std::vector<detail::ThreadRunner*> const& threads,
				neural_net::NeuralNetwork const& baseline,
				neural_net::NeuralNetwork const& competitor)
			{
				assert(threads.size() <= evaluators_.size());

				result_.Clear();
				next_show_ = std::chrono::steady_clock::now();
				auto cond = [&run_options, this] () mutable {
					std::lock_guard<std::mutex> lock(next_show_mutex_);
					auto now = std::chrono::steady_clock::now();
					if (now > next_show_) {
						next_show_ = now + std::chrono::milliseconds(run_options.show_interval_ms);
						logger_.Info([&](auto& s) {
							s << "Run " << result_.GetTotal() << " trials. Competitor wins " << result_.GetWin() << " times.";
						});
					}
					return result_.GetTotal() < run_options.runs;
				};

				for (size_t i = 0; i < threads.size(); ++i) {
					evaluators_[i].BeforeRun(baseline, competitor, result_);
				}

				for (size_t i = 0; i < threads.size(); ++i) {
					auto & evaluator = evaluators_[i];
					threads[i]->AsyncRunUnderCondition(cond, [&evaluator, &run_options, this](auto&& cb) {
						evaluator.Run(run_options, cb);
					});
				}

				running_evaluators_ = threads.size();
			}

			evaluation::CompetitionResult const& AfterRun() {
				return result_;
			}

		private:
			ILogger & logger_;
			std::vector<evaluation::Evaluator> evaluators_;
			evaluation::CompetitionResult result_;
			
			std::mutex next_show_mutex_;
			std::chrono::steady_clock::time_point next_show_;

			size_t running_evaluators_;
		};
	}
}