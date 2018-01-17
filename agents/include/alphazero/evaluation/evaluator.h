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
			Evaluator() : result_(), baseline_(), competitor_() {}

			Evaluator(Evaluator const&) = delete;
			Evaluator & operator=(Evaluator const&) = delete;

			Evaluator(Evaluator &&) = default;
			Evaluator & operator=(Evaluator &&) = default;

			void BeforeRun(neural_net::NeuralNetworkWrapper const& baseline, neural_net::NeuralNetworkWrapper const& competitor) {
				result_.Clear();
				baseline_.CopyFrom(baseline);
				competitor_.CopyFrom(competitor); // copy
			}

			void RunOnce(RunOptions const& options) {
				// TODO: compete several games
			}

			CompetitionResult AfterRun()
			{
				// TODO: fill result_
				return result_;
			}

		private:
			CompetitionResult result_;
			neural_net::NeuralNetworkWrapper baseline_;
			neural_net::NeuralNetworkWrapper competitor_;
		};
	}
}