#pragma once

#include "optimizer_options.h"
#include "neural_net.h"

namespace  alphazero
{
	namespace neural_net {
		class Optimizer
		{
		public:
			void Initialize() {
				// TODO: load existing model, or create a one with random weights
			}

			// Thread safety: 
			void RunOnce(OptimizerRunOptions const& options, NeuralNet & neural_net) {
				// TODO:
			}
		};
	}
}