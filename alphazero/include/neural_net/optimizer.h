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
			}

			// Thread safety: No
			void RunOnce(OptimizerRunOptions const& options, NeuralNet & neural_net) {
				// TODO:
			}
		};
	}
}