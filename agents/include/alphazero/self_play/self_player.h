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
#include "judge/json/Reader.h"
#include "TestStateBuilder.h"

namespace alphazero
{
	namespace self_play
	{
		class SelfPlayer
		{
		public:
			SelfPlayer(ILogger & logger, int rand_seed) :
				logger_(logger), random_(rand_seed),
				data_(nullptr), config_(), tmp_file_()
			{}
			~SelfPlayer() { RemoveTempFile(); }

			void BeforeRun(shared_data::TrainingData & data, neural_net::NeuralNetwork const& neural_net, RunOptions const& config) {
				data_ = &data;

				RemoveTempFile();
				tmp_file_ = std::tmpnam(nullptr);
				neural_net.Save(tmp_file_);

				result_.Clear();

				config_ = config;
				config_.agent_config.mcts.SetNeuralNetPath(tmp_file_);
				config_.agent_config.threads = 1;
				config_.agent_config.iterations_per_action = 1;
			}

			// Thread safety: No
			template <class Callback>
			void Run(Callback && callback) {
				// TODO: Choose actions proportions to its visit counts.
				// TODO: Add Dirchlet distribution to root node. This further ensures the variety.

				static_assert(std::is_same_v<
					mcts::StaticConfigs::SimulationPhaseSelectActionPolicy,
					mcts::policy::simulation::HeuristicPlayoutWithHeuristicEarlyCutoffPolicy>);

				auto start_board_seed = random_();
				auto start_board_getter = [&](std::mt19937 & random) -> state::State {
					return TestStateBuilder().GetStateWithRandomStartCard(start_board_seed, random);
				};

				while (callback()) {
					using MCTSAgent = agents::MCTSAgent<>;
					judge::json::Recorder recorder(random_);
					judge::Judger<MCTSAgent> judger(random_, recorder);
					MCTSAgent first(config_.agent_config);
					MCTSAgent second(config_.agent_config);

					judger.SetFirstAgent(&first);
					judger.SetSecondAgent(&second);

					judger.Start(start_board_getter, random_);

					judge::json::Reader reader;
					reader.Parse(recorder.GetJson(), [&](judge::json::NeuralNetInputGetter const& input, int label) {
						data_->Push(std::make_shared<shared_data::TrainingDataItem>(input, label));
						++result_.generated_count_;
					});
				}
			}

			RunResult AfterRun() {
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
			shared_data::TrainingData * data_;
			RunOptions config_;

			std::string tmp_file_;

			RunResult result_;
		};
	}
}