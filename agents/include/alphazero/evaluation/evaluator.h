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

			void BeforeRun(neural_net::NeuralNetwork const& baseline, neural_net::NeuralNetwork const& competitor) {
				result_.Clear();
				baseline_.CopyFrom(baseline);
				competitor_.CopyFrom(competitor); // copy
			}

			template <class Callback>
			void Run(RunOptions const& options, Callback&& callback) {
				while (callback()) {
					// TODO: compete several games
				}
			}

			CompetitionResult AfterRun()
			{
				// TODO: fill result_
				return result_;
			}

		private:
			CompetitionResult result_;
			neural_net::NeuralNetwork baseline_;
			neural_net::NeuralNetwork competitor_;
		};
	}
}