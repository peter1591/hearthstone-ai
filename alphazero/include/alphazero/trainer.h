#pragma once

#include <sstream>

#include "alphazero/detail/thread_pool.h"
#include "alphazero/shared_data/training_data.h"
#include "alphazero/neural_net/neural_net.h"
#include "alphazero/optimizer/runner.h"
#include "alphazero/self_play/runner.h"
#include "alphazero/evaluation/runner.h"
#include "alphazero/logger.h"

namespace alphazero
{
	struct TrainerConfigs {
		size_t kTrainingDataCapacityPowerOfTwo;
		int kNeuralNetTrainingBatch;

		// if new competitor's win rate larger than this one, use it to replace the best neural net so far
		float kEvaluationWinRate;
	};

	// Target to end-user devices. Eliminate context switch as much as possible.
	// Don't rely on threads and OS scheduling.
	// Not targeting for distributed environments.
	class EndDeviceTrainer {
	private:
		struct Schedule {
			int prepare_data_milliseconds;
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

		void Initialize(TrainerConfigs const& configs, std::string const& neural_net_model) {
			configs_ = configs;

			int kThreads = 4; // TODO: adjust at runtime

			schedule_.prepare_data_milliseconds = 100;
			schedule_.self_play_milliseconds = 3000;
			schedule_.neural_net_train_milliseconds = 3000;
			schedule_.evaluation_milliseconds = 3000;

			threads_.Initialize(kThreads);
			training_data_.Initialize(configs.kTrainingDataCapacityPowerOfTwo);

			if (neural_net_model.empty()) {
				neural_net_ = neural_net::NeuralNet::CreateRandomNeuralNet();
			}
			else {
				neural_net_ = neural_net::NeuralNet::LoadFromFile(neural_net_model);
			}
			best_neural_net_ = neural_net_;

			optimizer_.Initialize();
			evaluators_.Initialize(kThreads);
			self_players_.Initialize(kThreads, training_data_);
		}

		void Release() {
			threads_.Release();
		}

		void Train() {
			PrepareData();

			while (true) {
				AdjustRunOptions();

				TrainNeuralNetwork();

				EvaluateNeuralNetwork();

				SelfPlay();
			}
		}

	private:
		void AdjustRunOptions() {
			//schedule_.neural_net_train_milliseconds = 10 * 1000; // TODO: adjust at runtime
			//schedule_.evaluation_milliseconds = 10 * 1000; // TODO: adjust at runtime
		}

		void PrepareData() {
			logger_.Info() << "Start to prepare training data.";

			auto capacity = training_data_.GetCapacity();

			self_play::RunResult result;

			while (true) {
				result += InternalSelfPlay(schedule_.prepare_data_milliseconds);

				logger_.Info() << "Generated " << result.generated_count_ << " records.";

				if (result.generated_count_ > capacity) break;
			}
		}

		void SelfPlay() {
			logger_.Info() << "Start self play.";
			auto result = InternalSelfPlay(schedule_.self_play_milliseconds);
			logger_.Info() << "Generated " << result.generated_count_ << " records.";
		}

		self_play::RunResult InternalSelfPlay(int milliseconds) {
			std::vector<detail::ThreadRunner*> threads;
			for (size_t i = 0; i < threads_.Size(); ++i) {
				threads.push_back(&threads_.Get(i));
			}
			self_players_.BeforeRun(
				milliseconds,
				threads,
				neural_net_);

			for (auto thread : threads) thread->Wait();

			return self_players_.AfterRun();
		}

		void TrainNeuralNetwork() {
			logger_.Info() << "Start training neural network.";

			optimizer_.BeforeRun(
				schedule_.neural_net_train_milliseconds,
				&threads_.Get(0),
				configs_.kNeuralNetTrainingBatch,
				neural_net_,
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
				neural_net_);

			for (size_t i = 0; i < threads_.Size(); ++i) threads_.Get(i).Wait();

			optimizer_.AfterRun();
			auto self_player_result = self_players_.AfterRun();
			logger_.Info() << "Generated " << self_player_result.generated_count_ << " records.";
		}

		void EvaluateNeuralNetwork() {
			logger_.Info() << "Start evaluation neural network.";

			std::vector<detail::ThreadRunner*> threads;
			for (size_t i = 0; i < threads_.Size(); ++i) {
				threads.push_back(&threads_.Get(i));
			}
			evaluators_.BeforeRun(
				schedule_.evaluation_milliseconds,
				threads,
				best_neural_net_,
				neural_net_);

			for (auto thread : threads) thread->Wait();

			auto result = evaluators_.AfterRun();

			int win_threshold = (int)(configs_.kEvaluationWinRate * result.total_games);
			if (result.competitor_wins > win_threshold) {
				logger_.Info() << "Replace the best neural network with the new competitor!";
				best_neural_net_ = neural_net_;
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
		neural_net::NeuralNet best_neural_net_;
		neural_net::NeuralNet neural_net_;

		optimizer::Runner optimizer_;
		evaluation::Runner evaluators_;
		self_play::Runner self_players_;
	};
}