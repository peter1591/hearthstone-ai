#pragma once

#include <string>

namespace alphazero
{
	namespace neural_net {
		class NeuralNet
		{
		public:
			static NeuralNet LoadFromFile(std::string const& path) {
				// TODO
				return NeuralNet();
			}

			static NeuralNet CreateRandomNeuralNet() {
				// TODO
				return NeuralNet();
			}

		public:
			NeuralNet() {}

			NeuralNet(NeuralNet const& rhs) {
				// TODO: copy ctor
			}

			NeuralNet & operator=(NeuralNet const& rhs) {
				// TODO: copy assign
				return *this;
			}
		};
	}
}