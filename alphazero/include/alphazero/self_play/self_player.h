#pragma once

#include "alphazero/neural_net/neural_net.h"
#include "alphazero/self_play/self_player_options.h"
#include "alphazero/shared_data/training_data.h"

namespace alphazero
{
	namespace self_play
	{
		class SelfPlayer
		{
		public:
			void SetData(shared_data::TrainingDataWriter writer) {
				writer_ = writer;
			}

			// Thread safety: No
			void RunOnce(neural_net::NeuralNet const& neural_net, SelfPlayerRunOptions const& options) {
				// TODO: use MCTS to generate self play data
				// Choose actions proportions to its visit counts.
				// TODO: Add Dirchlet distribution to root node. This further ensures the variety.
			}

		private:
			shared_data::TrainingDataWriter writer_;
		};
	}
}