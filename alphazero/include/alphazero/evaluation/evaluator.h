#pragma once

#include "alphazero/neural_net/neural_net.h"
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

			void BeforeRun(neural_net::NeuralNet const& baseline, neural_net::NeuralNet const& competitor) {
				result_.Clear();
				baseline_ = baseline; // copy
				competitor_ = competitor; // copy
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
			neural_net::NeuralNet baseline_;
			neural_net::NeuralNet competitor_;
		};
	}
}