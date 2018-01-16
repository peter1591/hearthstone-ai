#pragma once

#include "alphazero/neural_net/options.h"
#include "alphazero/neural_net/neural_net.h"

namespace alphazero
{
	namespace neural_net {
		class Optimizer
		{
		public:
			void Initialize() {
			}

			void BeforeRun() {

			}

			// Thread safety: No
			void RunOnce(RunOptions const& options, NeuralNet & neural_net) {
				// TODO:
			}

			void AfterRun() {

			}
		};
	}
}