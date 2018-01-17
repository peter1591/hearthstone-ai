#pragma once

#include <vector>
#include <memory>

#include "neural_net/NeuralNetwork.h"
#include "alphazero/self_play/options.h"
#include "alphazero/self_play/result.h"
#include "alphazero/shared_data/training_data.h"
#include "alphazero/logger.h"

namespace alphazero
{
	namespace self_play
	{
		class SelfPlayer
		{
		public:
			SelfPlayer() : items_(), neural_net_(), data_(nullptr) {}

			void BeforeRun(shared_data::TrainingData & data, neural_net::NeuralNetworkWrapper const& neural_net) {
				assert(items_.empty());
				data_ = &data;
				neural_net_.CopyFrom(neural_net);
				result_.Clear();
			}

			// Thread safety: No
			void RunOnce(RunOptions const& options) {
				// TODO: use MCTS to generate self play data
				// Choose actions proportions to its visit counts.
				// TODO: Add Dirchlet distribution to root node. This further ensures the variety.

				for (int i = 0; i < 100; ++i) {
					items_.push_back(std::make_shared<shared_data::TrainingDataItem>());
					++result_.generated_count_;
				}

				data_->PushN(items_);
				assert(items_.empty());
			}

			RunResult AfterRun() {
				assert(items_.empty());
				return result_;
			}

		private:
			std::vector<std::shared_ptr<shared_data::TrainingDataItem>> items_;
			neural_net::NeuralNetworkWrapper neural_net_;
			shared_data::TrainingData * data_;

			RunResult result_;
		};
	}
}