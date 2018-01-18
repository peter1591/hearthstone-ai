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
			template <class Callback>
			void Run(RunOptions const& options, neural_net::NeuralNetwork & neural_net, Callback cb) {
				while (cb()) {
					// TODO:
				}
			}

			void AfterRun() {

			}
		};
	}
}