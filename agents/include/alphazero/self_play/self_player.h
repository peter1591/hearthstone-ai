#pragma once

#include <cstdio>
#include <chrono>
#include <vector>
#include <memory>

#include "engine/view/BoardRefView.h"
#include "neural_net/NeuralNetwork.h"
#include "alphazero/self_play/options.h"
#include "alphazero/self_play/result.h"
#include "alphazero/shared_data/training_data.h"
#include "alphazero/logger.h"

namespace alphazero
{
	namespace self_play
	{
		class AgentCallback
		{
		public:
			AgentCallback(int max_iterations) :
				first_time_(true), max_iterations_(max_iterations), last_shown_()
			{}

			void BeforeThink() {
				first_time_ = true;
			}

			void Thinking(engine::view::BoardRefView board_view, uint64_t iteration) {
				if (first_time_) {
					std::cout << "Turn: " << board_view.GetTurn() << std::endl;
					last_shown_ = std::chrono::steady_clock::now();
					first_time_ = false;
				}

				auto now = std::chrono::steady_clock::now();
				auto after_last_shown = std::chrono::duration_cast<std::chrono::seconds>(now - last_shown_).count();
				if (after_last_shown > 5) {
					double percent = (double)iteration / max_iterations_;
					std::cout << "Iterations: " << iteration << " (" << percent * 100.0 << "%)" << std::endl;
					last_shown_ = now;
				}
			}

			void AfterThink(uint64_t iteration) {
				std::cout << "Total iterations: " << iteration << std::endl;
			}

		private:
			bool first_time_;
			uint64_t max_iterations_;
			std::chrono::steady_clock::time_point last_shown_;
		};

		class SelfPlayer
		{
		public:
			SelfPlayer(ILogger & logger) : logger_(logger), items_(), data_(nullptr), config_(), tmp_file_() {}
			~SelfPlayer() { RemoveTempFile(); }

			void BeforeRun(shared_data::TrainingData & data, neural_net::NeuralNetwork const& neural_net, RunOptions const& config) {
				assert(items_.empty());
				data_ = &data;

				RemoveTempFile();
				tmp_file_ = std::tmpnam(nullptr);
				neural_net.Save(tmp_file_);

				result_.Clear();

				config_ = config;
				config_.agent_config.mcts.SetNeuralNetPath(tmp_file_);
			}

			// Thread safety: No
			template <class Callback>
			void Run(Callback && callback) {
				// TODO: use MCTS to generate self play data
				// Choose actions proportions to its visit counts.
				// TODO: Add Dirchlet distribution to root node. This further ensures the variety.

				static_assert(std::is_same_v<
					mcts::StaticConfigs::SimulationPhaseSelectActionPolicy,
					mcts::policy::simulation::HeuristicPlayoutWithHeuristicEarlyCutoffPolicy>);

				while (callback()) {
					for (int i = 0; i < 100; ++i) {
						items_.push_back(std::make_shared<shared_data::TrainingDataItem>());
						++result_.generated_count_;
					}

					data_->PushN(items_);
					assert(items_.empty());

					logger_.Info("hello");
				}
			}

			RunResult AfterRun() {
				assert(items_.empty());
				RemoveTempFile();
				return result_;
			}

		private:
			void RemoveTempFile() {
				if (!tmp_file_.empty()) {
					std::remove(tmp_file_.c_str());
					tmp_file_.clear();
				}
			}

		private:
			ILogger & logger_;
			std::vector<std::shared_ptr<shared_data::TrainingDataItem>> items_;
			shared_data::TrainingData * data_;
			RunOptions config_;

			std::string tmp_file_;

			RunResult result_;
		};
	}
}