#pragma once

#include <atomic>
#include <chrono>
#include <thread>

#include "state/State.h"
#include "engine/view/BoardView.h"
#include "engine/view/board_view/StateRestorer.h"
#include "MCTS/MOMCTS.h"
#include "judge/IAgent.h"
#include "agents/MCTSRunner.h"
#include "agents/MCTSConfig.h"

namespace agents
{
	class DummyAgentCallback
	{
	public:
		DummyAgentCallback() {}

		void BeforeThink() {}
		void Thinking(engine::view::BoardRefView board_view, uint64_t iteration) {}
		void AfterThink(uint64_t iteration) {}
	};

	template <class AgentCallback = DummyAgentCallback>
	class MCTSAgent {
	public:
		MCTSAgent(MCTSAgentConfig const& config, AgentCallback cb = AgentCallback()) :
			config_(config),
			root_node_(nullptr), node_(nullptr), controller_(),
			cb_(cb)
		{}

		MCTSAgent(MCTSAgent const&) = delete;
		MCTSAgent & operator=(MCTSAgent const&) = delete;

		void Think(state::PlayerIdentifier side, engine::view::BoardRefView const& game_state , std::mt19937 & random) {
			cb_.BeforeThink();

			controller_.reset(new MCTSRunner(config_, random));
			controller_->Run(game_state);

			while (true) {
				uint64_t iterations = controller_->GetStatistic().GetSuccededIterates();
				cb_.Thinking(game_state, iterations);
				if (iterations >= config_.iterations_per_action) break;

				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
			controller_->WaitUntilStopped();

			cb_.AfterThink(controller_->GetStatistic().GetSuccededIterates());

			node_ = controller_->GetRootNode(side);
			root_node_ = node_;
		}

		int GetAction(engine::ActionType::Types action_type, engine::ActionChoices action_choices) {
			if (action_type != engine::ActionType::kMainAction)
			{
				assert(action_choices.Size() > 0);
				if (action_choices.Size() == 1) return 0;
			}

			assert(node_);

			if (!node_->addon_.consistency_checker.CheckActionType(action_type)) {
				assert(false);
				throw std::runtime_error("Action type not match");
			}

			auto CanBeChosen = [&](int choice) {
				for (action_choices.Begin(); !action_choices.IsEnd(); action_choices.StepNext()) {
					if (action_choices.Get() == choice) return true;
				}
				return false;
			};

			int best_choice = -1;
			double best_chosen_times = -std::numeric_limits<double>::infinity();
			mcts::selection::TreeNode const* best_node = nullptr;
			node_->children_.ForEach([&](int choice, mcts::selection::EdgeAddon const* edge_addon, mcts::selection::TreeNode * child) {
				if (!CanBeChosen(choice)) return true;

				double chosen_times = (double)edge_addon->GetChosenTimes();
				if (chosen_times > best_chosen_times) {
					best_chosen_times = chosen_times;
					best_choice = choice;
					best_node = child;
				}
				return true;
			});

			if (best_choice < 0) {
				throw std::runtime_error("No any choice is evaluated.");
			}

			node_ = best_node;
			return best_choice;
		}

	private:
		MCTSAgentConfig config_;
		mcts::selection::TreeNode const* root_node_;
		mcts::selection::TreeNode const* node_;
		std::unique_ptr<MCTSRunner> controller_;
		AgentCallback cb_;
	};
}