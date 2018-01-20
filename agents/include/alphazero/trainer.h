#pragma once

#include <sstream>

#include "alphazero/detail/thread_pool.h"
#include "alphazero/shared_data/training_data.h"
#include "alphazero/optimizer/runner.h"
#include "alphazero/self_play/runner.h"
#include "alphazero/evaluation/runner.h"
#include "alphazero/logger.h"
#include "neural_net/NeuralNetwork.h"

namespace alphazero
{
	struct TrainerConfigs {
		std::string best_net_path_;
		std::string competitor_net_path_;

		size_t kTrainingDataCapacityPowerOfTwo;

		// Need at least this number of training data to start training
		size_t kMinimumTraningData;

		optimizer::RunOptions optimizer;

		// if new competitor's win rate larger than this one, use it to replace the best neural net so far
		float kEvaluationWinRate;
	};

	// Target to end-user devices. Eliminate context switch as much as possible.
	// Don't rely on threads and OS scheduling.
	// Not targeting for distributed environments.
	class EndDeviceTrainer {
	private:
		struct Schedule {
			int self_play_milliseconds;
			int neural_net_train_milliseconds;
			int evaluation_milliseconds;
		};

	public:
		EndDeviceTrainer(ILogger & logger, std::mt19937 & random) :
			logger_(logger),
			configs_(),
			random_(random),
			schedule_(),
			threads_(),
			training_data_(),
			best_neural_net_(),
			neural_net_(),
			optimizer_(logger),
			evaluators_(logger),
			self_players_(logger)
		{}

		void Initialize(TrainerConfigs const& configs, std::mt19937 & random) {
			configs_ = configs;

			int kThreads = 4; // TODO: adjust at runtime

			schedule_.self_play_milliseconds = 3000;
			schedule_.neural_net_train_milliseconds = 3000;
			schedule_.evaluation_runs = 1000;

			threads_.Initialize(kThreads);
			training_data_.Initialize(configs.kTrainingDataCapacityPowerOfTwo);

			best_neural_net_.Load(configs.best_net_path_);
			neural_net_.Load(configs.best_net_path_);

			optimizer_.Initialize();
			evaluators_.Initialize(kThreads);
			self_players_.Initialize(kThreads, training_data_, random);
		}

		void Release() {
			threads_.Release();
		}

		void Train() {
			PrepareData();

			while (true) {
				AdjustSchedule();

				TrainNeuralNetwork();

				EvaluateNeuralNetwork();

				SelfPlay();
			}
		}

	private:
		void AdjustSchedule() {
			//schedule_.neural_net_train_milliseconds = 10 * 1000; // TODO: adjust at runtime
			//schedule_.evaluation_milliseconds = 10 * 1000; // TODO: adjust at runtime
		}

		void PrepareData() {
			logger_.Info() << "Start to prepare training data.";

			auto capacity = training_data_.GetCapacity();

			self_play::RunResult result;

			std::mutex next_show_mutex;
			auto next_show = std::chrono::steady_clock::now();
			auto condition = [&next_show_mutex, &next_show, this]() mutable -> bool {
				auto records = training_data_.GetSize();
				
				bool show = false;
				{
					std::lock_guard<std::mutex> lock(next_show_mutex);
					auto now = std::chrono::steady_clock::now();
					if (now > next_show) {
						next_show = now + std::chrono::seconds(5);
						show = true; // no need to logger lock here, so use a flag to release lock
					}
				}
				if (show) {
					logger_.Info([&](auto& s) {
						s << "Generated " << records << " / " << configs_.kMinimumTraningData << " records.";
					});
				}
				return records < configs_.kMinimumTraningData;
			};

			InternalSelfPlay(condition);
		}

		void SelfPlay() {
			logger_.Info() << "Start self play.";
			
			auto start = std::chrono::steady_clock::now();
			auto until = start + std::chrono::milliseconds(schedule_.self_play_milliseconds);
			auto result = InternalSelfPlay([until]() {
				return std::chrono::steady_clock::now() < until;
			});
			logger_.Info() << "Generated " << result.generated_count_ << " records.";
		}

		self_play::RunResult InternalSelfPlay(detail::ThreadRunner::ConditionCallback condition) {
			std::vector<detail::ThreadRunner*> threads;
			for (size_t i = 0; i < threads_.Size(); ++i) {
				threads.push_back(&threads_.Get(i));
			}
			self_players_.BeforeRun(
				condition,
				threads,
				best_neural_net_);

			for (auto thread : threads) thread->Wait();

			return self_players_.AfterRun();
		}

		void TrainNeuralNetwork() {
			logger_.Info() << "Start training neural network.";

			optimizer_.BeforeRun(
				schedule_.neural_net_train_milliseconds,
				configs_.optimizer,
				&threads_.Get(0),
				best_neural_net_,
				training_data_,
				random_);

			// Utilize the rest threads to generate self-play data
			std::vector<detail::ThreadRunner*> threads;
			for (size_t i = 1; i < threads_.Size(); ++i) {
				threads.push_back(&threads_.Get(i));
			}
			self_players_.BeforeRun(
				schedule_.self_play_milliseconds,
				threads,
				best_neural_net_);

			for (size_t i = 0; i < threads_.Size(); ++i) threads_.Get(i).Wait();

			optimizer_.AfterRun();
			neural_net_.Save(configs_.competitor_net_path_);
			logger_.Info() << "Saved trained neural net as competitor. "
				<< "(path=" << configs_.competitor_net_path_ << ")";

			auto self_player_result = self_players_.AfterRun();
			logger_.Info() << "Generated " << self_player_result.generated_count_ << " records.";
		}

		void EvaluateNeuralNetwork() {
			logger_.Info() << "Start evaluation neural network.";

			evaluation::RunOptions options;
			options.runs = schedule_.evaluation_runs;

			std::vector<detail::ThreadRunner*> threads;
			for (size_t i = 0; i < threads_.Size(); ++i) {
				threads.push_back(&threads_.Get(i));
			}
			evaluators_.BeforeRun(
				options,
				threads,
				best_neural_net_,
				neural_net_);

			for (auto thread : threads) thread->Wait();

			auto const& result = evaluators_.AfterRun();

			int win_threshold = (int)(configs_.kEvaluationWinRate * result.GetTotal());
			if (result.GetWin() > win_threshold) {
				logger_.Info() << "Replace the best neural network with the new competitor!";
				best_neural_net_.CopyFrom(neural_net_);
			}
			else {
				logger_.Info() << "Competitor not strong enough. Continue to use the best neural network so far.";
			}
		}

	private:
		ILogger & logger_;
		TrainerConfigs configs_;
		std::mt19937 & random_;

		Schedule schedule_;
		detail::ThreadPool threads_;

		shared_data::TrainingData training_data_;
		neural_net::NeuralNetwork best_neural_net_;
		neural_net::NeuralNetwork neural_net_;

		optimizer::Runner optimizer_;
		evaluation::Runner evaluators_;
		self_play::Runner self_players_;
	};
}