#pragma once

#include <cstdio>
#include <chrono>
#include <vector>
#include <memory>
#include <random>
#include <fstream>
#include <time.h>

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
		class AgentCallback
		{
		public:
			AgentCallback(ILogger & logger) : logger_(logger), next_show_(), show_interval_() {
				show_interval_ = std::chrono::milliseconds(5000);
			}

			void BeforeThink(engine::view::BoardRefView const& board_view) {
				logger_.Info([&](auto& s) {
					s << "Start thinking... Turn: " << board_view.GetTurn() << ". ";
				});
				next_show_ = std::chrono::steady_clock::now() + show_interval_;
			}
			void Thinking(engine::view::BoardRefView const& board_view, uint64_t iteration) {
				auto now = std::chrono::steady_clock::now();
				if (now > next_show_) {
					logger_.Info([&](auto& s) {
						s << "Iterations: " << iteration;
					});
					next_show_ = now + show_interval_;
				}
			}
			void AfterThink(uint64_t iteration) {}

		private:
			ILogger & logger_;
			std::chrono::steady_clock::time_point next_show_;
			std::chrono::milliseconds show_interval_;
		};

		class SelfPlayer
		{
		public:
			SelfPlayer(ILogger & logger, int rand_seed) :
				logger_(logger), random_(rand_seed),
				data_(nullptr), config_(), tmp_file_(),
				result_()
			{}
			~SelfPlayer() { RemoveTempFile(); }

			SelfPlayer(SelfPlayer const&) = delete;
			SelfPlayer & operator=(SelfPlayer const&) = delete;

			SelfPlayer(SelfPlayer &&) = default;
			SelfPlayer & operator=(SelfPlayer &&) = default;

			void BeforeRun(shared_data::TrainingData & data, neural_net::NeuralNetwork const& neural_net, RunOptions const& config) {
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
				// TODO: Choose actions proportions to its visit counts.
				// TODO: Add Dirchlet distribution to root node. This further ensures the variety.

				static_assert(std::is_same_v<
					mcts::StaticConfigs::SimulationPhaseSelectActionPolicy,
					mcts::policy::simulation::HeuristicPlayoutWithHeuristicEarlyCutoffPolicy>);

				while (callback()) {
					auto hand_card_seed = random_();
					state::State start_state =
						TestStateBuilder().GetStateWithRandomStartCard(hand_card_seed, random_);

					using MCTSAgent = agents::MCTSAgent<AgentCallback>;
					judge::json::Recorder recorder(random_);
					judge::Judger<MCTSAgent, judge::json::Recorder> judger(random_, recorder);
					MCTSAgent first(config_.agent_config, AgentCallback(logger_));
					MCTSAgent second(config_.agent_config, AgentCallback(logger_));

					judger.SetFirstAgent(&first);
					judger.SetSecondAgent(&second);

					judger.Start(start_state, random_);

					SaveJson(recorder.GetJson());

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

			void SaveJson(Json::Value const& json) {
				if (config_.save_dir.empty()) return;

				time_t now;
				time(&now);

				struct tm timeinfo;
#ifdef _MSC_VER
				localtime_s(&timeinfo, &now);
#else
				localtime_r(&now, &timeinfo);
#endif

				char buffer[80];
				strftime(buffer, 80, "%Y%m%d-%H%M%S", &timeinfo);

				std::ostringstream ss;
				int postfix = rand() % 90000 + 10000;
				ss << config_.save_dir << "/" << buffer << "-" << postfix << ".json";
				std::string filename = ss.str();

				std::ofstream fs(filename, std::ofstream::trunc);
				Json::StyledStreamWriter json_writer;
				json_writer.write(fs, json);
				fs.close();
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
