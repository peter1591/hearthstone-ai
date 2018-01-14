#pragma once

#include "neural_net/neural_net.h"
#include "self_play/self_player_options.h"

namespace alphazero
{
	namespace self_play
	{
		class SelfPlayer
		{
		public:
			// Thread safety: No
			void RunOnce(neural_net::NeuralNet const& neural_net, SelfPlayerRunOptions const& options) {
				// TODO: use MCTS to generate self play data
				// Choose actions proportions to its visit counts. This further ensures the variety.
			}

		private:
		};
	}
}