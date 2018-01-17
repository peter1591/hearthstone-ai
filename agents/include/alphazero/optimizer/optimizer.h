#pragma once

#include "neural_net/NeuralNetwork.h"
#include "alphazero/optimizer/options.h"

namespace alphazero
{
	namespace optimizer {
		class Optimizer
		{
		public:
			void Initialize() {
			}

			void BeforeRun() {

			}

			// Thread safety: No
			void RunOnce(RunOptions const& options, neural_net::NeuralNetwork & neural_net) {
				// TODO:
			}

			void AfterRun() {

			}
		};
	}
}