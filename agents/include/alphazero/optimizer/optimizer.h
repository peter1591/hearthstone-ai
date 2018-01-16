#pragma once

#include "alphazero/neural_net/neural_net.h"
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
			void RunOnce(RunOptions const& options, neural_net::NeuralNet & neural_net) {
				// TODO:
			}

			void AfterRun() {

			}
		};
	}
}