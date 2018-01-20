#pragma once

#include "neural_net/NeuralNetwork.h"
#include "alphazero/evaluation/competition_result.h"
#include "alphazero/evaluation/options.h"

namespace alphazero
{
	namespace evaluation
	{
		class Evaluator
		{
		public:
			Evaluator(int rand_seed) :
				result_(nullptr), best_net_path_(), competitor_net_path_(), random_(rand_seed)
			{}

			Evaluator(Evaluator const&) = delete;
			Evaluator & operator=(Evaluator const&) = delete;

			Evaluator(Evaluator &&) = default;
			Evaluator & operator=(Evaluator &&) = default;

			void BeforeRun(
				std::string const& best_net_path,
				std::string const& competitor_net_path,
				CompetitionResult & result)
			{
				result_ = &result;
				best_net_path_ = best_net_path;
				competitor_net_path_ = competitor_net_path;
			}

			template <class Callback>
			void Run(RunOptions const& options, Callback&& callback) {
				agents::MCTSAgentConfig best_agent_config = options.agent_config;
				best_agent_config.mcts.SetNeuralNetPath(best_net_path_);

				agents::MCTSAgentConfig competitor_agent_config = options.agent_config;
				competitor_agent_config.mcts.SetNeuralNetPath(competitor_net_path_);

				while (callback()) {
					auto start_board_seed = random_();
					auto start_board_getter = [&](std::mt19937 & random) -> state::State {
						return TestStateBuilder().GetStateWithRandomStartCard(start_board_seed, random);
					};

					using MCTSAgent = agents::MCTSAgent<>;
					judge::NullRecorder recorder;
					judge::Judger<MCTSAgent> judger(random_, recorder);
					MCTSAgent best_agent(best_agent_config);
					MCTSAgent competitor_agent(competitor_agent_config);

					bool competitor_go_first = ((random_() % 2) == 0);
					engine::Result competitor_win_result;

					if (competitor_go_first) {
						judger.SetFirstAgent(&competitor_agent);
						judger.SetSecondAgent(&best_agent);
						competitor_win_result = engine::kResultFirstPlayerWin;
					}
					else {
						judger.SetFirstAgent(&best_agent);
						judger.SetSecondAgent(&competitor_agent);
						competitor_win_result = engine::kResultSecondPlayerWin;
					}

					engine::Result result = judger.Start(start_board_getter, random_);
					bool competitor_win = (result == competitor_win_result);

					result_->AddResult(competitor_win);
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
				}
			}

			void AfterRun()
			{
			}

		private:
			CompetitionResult * result_;
			std::string best_net_path_;
			std::string competitor_net_path_;
			std::mt19937 random_;
		};
	}
}