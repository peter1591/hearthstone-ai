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
				logger_(logger), optimizer_(), input_(), output_()
			{}

			void Initialize()
			{
				optimizer_.Initialize();
			}

			void BeforeRun(
				int milliseconds,
				RunOptions const& options,
				detail::ThreadRunner* thread,
				neural_net::NeuralNetwork & neural_net,
				shared_data::TrainingData & training_data,
				std::mt19937 & random)
			{
				auto start = std::chrono::steady_clock::now();
				auto until = start + std::chrono::milliseconds(milliseconds);

				logger_.Info() << "Preparing training data...";

				input_.Clear();
				output_.Clear();

				int fetched = 0;
				int rest_tries = options.batches * options.batch_size;
				int allowed_fetch_failures = (int)(options.maximum_fetch_failure_rate * rest_tries);

				while (--rest_tries >= 0) {
					bool success = training_data.RandomGet(random, [&](shared_data::TrainingDataItem const& item) {
						input_.AddData(&item.GetInput());
						output_.AddData(item.GetLabel());
						++fetched;
					});
					if (!success) {
						if (--allowed_fetch_failures < 0) {
							logger_.Info() << "Failed to fetch training data. (Too high failure rate).";
							return;
						}
					}
				}

				if (fetched < options.batch_size) {
					logger_.Info() << "Not enough training data for batch.";
					return;
				}

				logger_.Info() << "Training neural network... (fetched " << fetched << " data)";
				optimizer_.BeforeRun();
				thread->AsyncRunUntil(until, [&](auto&& cb) {
					int epoch = 0;
					auto cb2 = [&]() {
						epoch += options.epochs_per_run;
						logger_.Info() << "Trained " << epoch << " epoches.";
						return cb();
					};
					optimizer_.Run(input_, output_, options, neural_net, cb2);
				});
			}

			void AfterRun() {
				optimizer_.AfterRun();
			}

		private:
			ILogger & logger_;
			Optimizer optimizer_;
			neural_net::NeuralNetworkInput input_;
			neural_net::NeuralNetworkOutput output_;
		};
	}
}