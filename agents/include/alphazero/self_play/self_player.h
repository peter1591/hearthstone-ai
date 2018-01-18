#pragma once

#include <cstdio>
#include <chrono>
#include <vector>
#include <memory>
#include <random>

#include "engine/Game-impl.h"
#include "agents/MCTSAgent.h"
#include "engine/view/BoardRefView.h"
#include "neural_net/NeuralNetwork.h"
#include "alphazero/self_play/options.h"
#include "alphazero/self_play/result.h"
#include "alphazero/shared_data/training_data.h"
#include "alphazero/logger.h"
#include "judge/Judger.h"
#include "TestStateBuilder.h"

namespace alphazero
{
	namespace self_play
	{
		class AgentCallback
		{
		public:
			AgentCallback(ILogger & logger) :
				logger_(logger), first_time_(true), last_shown_()
			{}

			void BeforeThink() {
				first_time_ = true;
			}

			void Thinking(engine::view::BoardRefView board_view, uint64_t iteration) {
				if (first_time_) {
					logger_.Info([&](auto& s) {
						s << "Turn: " << board_view.GetTurn();
					});
					last_shown_ = std::chrono::steady_clock::now();
					first_time_ = false;
				}

				auto now = std::chrono::steady_clock::now();
				auto after_last_shown = std::chrono::duration_cast<std::chrono::seconds>(now - last_shown_).count();
				if (after_last_shown > 5) {
					logger_.Info([&](auto& s) {
						s << "Iterations: " << iteration;
					});
					last_shown_ = now;
				}
			}

			void AfterThink(uint64_t iteration) {
			}

		private:
			ILogger & logger_;
			bool first_time_;
			std::chrono::steady_clock::time_point last_shown_;
		};

		class SelfPlayer
		{
		public:
			SelfPlayer(ILogger & logger, int rand_seed) :
				logger_(logger), random_(rand_seed),
				items_(), data_(nullptr), config_(), tmp_file_()
			{}
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
				config_.agent_config.threads = 1;
				config_.agent_config.iterations_per_action = 10;
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

				using MCTSAgent = agents::MCTSAgent<AgentCallback>;
				judge::JsonRecorder recorder(random_);
				judge::Judger<MCTSAgent> judger(random_, recorder);
				MCTSAgent first(config_.agent_config, AgentCallback(logger_));
				MCTSAgent second(config_.agent_config, AgentCallback(logger_));

				auto start_board_seed = random_();
				auto start_board_getter = [&](std::mt19937 & random) -> state::State {
					return TestStateBuilder().GetStateWithRandomStartCard(start_board_seed, random);
				};

				judger.SetFirstAgent(&first);
				judger.SetSecondAgent(&second);

				while (callback()) {
					judger.Start(start_board_getter, random_);

					for (int i = 0; i < 100; ++i) {
						items_.push_back(std::make_shared<shared_data::TrainingDataItem>());
						++result_.generated_count_;
					}

					data_->PushN(items_);
					assert(items_.empty());
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
			std::mt19937 random_;
			std::vector<std::shared_ptr<shared_data::TrainingDataItem>> items_;
			shared_data::TrainingData * data_;
			RunOptions config_;

			std::string tmp_file_;

			RunResult result_;
		};
	}
}