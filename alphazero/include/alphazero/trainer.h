#pragma once

#include "alphazero/detail/thread_pool.h"
#include "alphazero/shared_data/training_data.h"
#include "alphazero/neural_net/neural_net.h"
#include "alphazero/neural_net/optimizer.h"
#include "alphazero/runner.h"
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
			runner_(),
			training_data_(),
			best_neural_nets_(),
			neural_net_(),
			optimizer_()
		{}

		void Initialize(TrainerConfigs const& configs, std::string const& neural_net_model) {
			configs_ = configs;

			int kThreads = 4; // TODO: adjust at runtime

			schedule_.self_play_milliseconds = 3000;
			schedule_.neural_net_train_milliseconds = 3000;
			schedule_.evaluation_milliseconds = 3000;

			threads_.Initialize(kThreads);
			runner_.Initialize();
			training_data_.Initialize(configs.kTrainingDataCapacityPowerOfTwo);

			if (neural_net_model.empty()) {
				neural_net_ = neural_net::NeuralNet::CreateRandomNeuralNet();
			}
			else {
				neural_net_ = neural_net::NeuralNet::LoadFromFile(neural_net_model);
			}

			optimizer_.Initialize();

			for (size_t i = 0; i < threads_.Size(); ++i) {
				best_neural_nets_.emplace_back(neural_net_);
				evaluators_.emplace_back();
				self_players_.emplace_back(logger_);
			}
		}

		void Release() {
			threads_.Release();
		}

		void Train() {
			// warm up: fill up the training data
			// TODO: wait until traiing data are fully filled
			SelfPlay();

			while (true) {
				std::this_thread::sleep_for(std::chrono::seconds(0));
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

		void SelfPlay() {
			std::vector<size_t> thread_ids;
			for (size_t i = 0; i < threads_.Size(); ++i) {
				thread_ids.push_back(i);
			}

			BeforeSelfPlay(thread_ids);
			runner_.WaitAll();
			AfterSelfPlay(thread_ids);
		}

		void BeforeSelfPlay(std::vector<size_t> const& threads) {
			logger_.Info("Start self play.");

			for (size_t tid : threads) self_players_[tid].BeforeRun(training_data_);

			for (size_t tid : threads) {
				runner_.AsyncRun(
					threads_.Get(tid),
					schedule_.self_play_milliseconds,
					self_players_[tid],
					best_neural_nets_[tid]);
			}
		}

		void AfterSelfPlay(std::vector<size_t> const& threads) {
			for (size_t tid : threads) self_players_[tid].AfterRun();
		}

		void TrainNeuralNetwork() {
			logger_.Info("Start training neural network.");

			auto start = std::chrono::steady_clock::now();
			int rest_ms = schedule_.neural_net_train_milliseconds;
			int past_ms = 0;
			std::vector<shared_data::TrainingDataItem> data;
			logger_.Info("Preparing training data...");
			int i = 0;
			while (data.size() < configs_.kNeuralNetTrainingBatch) {
				training_data_.RandomGet(random_, [&](shared_data::TrainingDataItem const& item) {
					data.push_back(item); // copy
				});

				++i;
				if (i % 10000 == 0) {
					auto now = std::chrono::steady_clock::now();
					past_ms = (int)std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
					if (past_ms > rest_ms) {
						logger_.Info("Failed to prepare training data. Maybe the data is not large enough.");
						return;
					}
				}
			}
			rest_ms -= past_ms;

			logger_.Info("Training neural network...");
			// Train neural network in the first thread
			// TODO: can tiny_dnn be trained at multiple threads?
			runner_.AsyncRun(
				threads_.Get(0),
				schedule_.neural_net_train_milliseconds,
				optimizer_,
				neural_net_);

			// Utilize the rest threads to generate self-play data
			std::vector<size_t> thread_ids;
			for (size_t i = 1; i < threads_.Size(); ++i) {
				thread_ids.push_back(i);
			}
			BeforeSelfPlay(thread_ids);

			runner_.WaitAll();

			AfterSelfPlay(thread_ids);
		}

		void EvaluateNeuralNetwork() {
			logger_.Info("Start evaluation neural network.");

			std::vector<neural_net::NeuralNet> copied_neural_net;

			for (size_t i = 0; i < threads_.Size(); ++i) {
				// TODO: if neural network can be shared safely across multiple threads, we can safe this copy
				copied_neural_net.push_back(neural_net_); // by copy

				runner_.AsyncRun(
					threads_.Get(i),
					schedule_.evaluation_milliseconds,
					evaluators_[i],
					best_neural_nets_[i],
					copied_neural_net[i]);
			}
			
			runner_.WaitAll();

			evaluation::CompetitionResult result;
			for (size_t i = 0; i < threads_.Size(); ++i) {
				result += evaluators_[i].GetCompetitionResult();
			}

			int win_threshold = (int)(configs_.kEvaluationWinRate * result.total_games);
			if (result.competitor_wins > win_threshold) {
				logger_.Info("Replace the best neural network with the new competitor!");
				for (auto & best_net : best_neural_nets_) {
					// TODO: if neural net can be shared safely across multiple threads, we can save this copy
					best_net = neural_net_;
				}
			}
			else {
				logger_.Info("Competitor not strong enough. Continue to use the best neural network so far.");
			}
		}

	private:
		ILogger & logger_;
		TrainerConfigs configs_;
		std::mt19937 & random_;

		Schedule schedule_;
		detail::ThreadPool threads_;
		Runner runner_;
		shared_data::TrainingData training_data_;

		// one for each thread
		// TODO: if neural net can be shared safely across multiple threads, we can save this
		std::vector<neural_net::NeuralNet> best_neural_nets_;
		
		// training neural net
		neural_net::NeuralNet neural_net_;

		neural_net::Optimizer optimizer_;

		std::vector<evaluation::Evaluator> evaluators_;

		std::vector<self_play::SelfPlayer> self_players_;
	};
}