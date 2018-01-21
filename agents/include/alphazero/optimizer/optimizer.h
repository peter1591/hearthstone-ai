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
			void Run(
				neural_net::NeuralNetworkInput const& input, neural_net::NeuralNetworkOutput const& output,
				RunOptions const& options, neural_net::NeuralNetwork & neural_net, Callback cb)
			{
				while (cb()) {
					// TODO: Train tiny_dnn in multiple threads
					neural_net.Train(
						input,
						output,
						options.batch_size,
						options.epoches_per_run);
				}
			}

			void AfterRun() {

			}
		};
	}
}