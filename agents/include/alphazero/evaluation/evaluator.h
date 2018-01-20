#pragma once

#include "neural_net/NeuralNetwork.h"
#include "alphazero/evaluation/competition_result.h"
#include "alphazero/evaluation/options.h"

namespace alphazero
{
	namespace evaluation
	{
		class Evaluator
		{
		public:
			Evaluator() : result_(nullptr), baseline_(), competitor_() {}

			Evaluator(Evaluator const&) = delete;
			Evaluator & operator=(Evaluator const&) = delete;

			Evaluator(Evaluator &&) = default;
			Evaluator & operator=(Evaluator &&) = default;

			void BeforeRun(
				neural_net::NeuralNetwork const& baseline,
				neural_net::NeuralNetwork const& competitor,
				CompetitionResult & result)
			{
				result_ = &result;
				baseline_.CopyFrom(baseline);
				competitor_.CopyFrom(competitor); // copy
			}

			template <class Callback>
			void Run(RunOptions const& options, Callback&& callback) {
				while (callback()) {
					// TODO: compete several games
					result_->AddResult(false);
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
				}
			}

			void AfterRun()
			{
			}

		private:
			CompetitionResult * result_;
			neural_net::NeuralNetwork baseline_;
			neural_net::NeuralNetwork competitor_;
		};
	}
}