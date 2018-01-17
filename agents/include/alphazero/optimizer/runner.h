#pragma once

#include <assert.h>
#include <chrono>
#include <vector>

#include "alphazero/logger.h"
#include "alphazero/detail/thread_runner.h"
#include "alphazero/optimizer/optimizer.h"
#include "alphazero/shared_data/training_data.h"

namespace alphazero
{
	namespace optimizer
	{
		class Runner
		{
		public:
			Runner(ILogger & logger) :
				logger_(logger),
				run_options_()
			{}

			void Initialize()
			{
				optimizer_.Initialize();
			}

			void BeforeRun(
				int milliseconds,
				detail::ThreadRunner* thread,
				int batch_size,
				neural_net::NeuralNetworkWrapper & neural_net,
				shared_data::TrainingData & training_data,
				std::mt19937 & random)
			{
				auto start = std::chrono::steady_clock::now();
				auto until = start + std::chrono::milliseconds(milliseconds);

				std::vector<shared_data::TrainingDataItem> data;
				logger_.Info() << "Preparing training data...";
				int i = 0;
				for (int i = 0; i < batch_size; ++i) {
					training_data.RandomGet(random, [&](shared_data::TrainingDataItem const& item) {
						data.push_back(item); // copy
					});
				}

				logger_.Info() << "Training neural network...";
				// Train neural network in the first thread
				// TODO: can tiny_dnn be trained at multiple threads?
				optimizer_.BeforeRun();
				thread->AsyncRunUntil(until, [&]() {
					optimizer_.RunOnce(run_options_, neural_net);
				});
			}

			void AfterRun() {
				optimizer_.AfterRun();
			}

		private:
			ILogger & logger_;
			RunOptions run_options_;
			Optimizer optimizer_;
		};
	}
}