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

		void BeforeThink(engine::view::BoardRefView const& board_view) {}
		void Thinking(engine::view::BoardRefView const& board_view, uint64_t iteration) {}
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

		void Think(engine::view::BoardRefView const& game_state, std::mt19937 & random) {
			cb_.BeforeThink(game_state);

			controller_.reset(new MCTSRunner(config_, random));
			controller_->Run(game_state);

			while (true) {
				uint64_t iterations = controller_->GetStatistic().GetSuccededIterates();
				cb_.Thinking(game_state, iterations);
				if (iterations >= (uint64_t)config_.iterations_per_action) break;

				std::this_thread::sleep_for(std::chrono::milliseconds(config_.callback_interval_ms));
			}
			controller_->WaitUntilStopped();

			cb_.AfterThink(controller_->GetStatistic().GetSuccededIterates());

			node_ = controller_->GetRootNode(game_state.GetSide());
			root_node_ = node_;
		}

		int GetAction(engine::ActionType::Types action_type, engine::ActionChoices action_choices, std::mt19937 & random) {
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

			struct Item {
				double value;
				int choice;
				mcts::selection::TreeNode const* node;
			};
			std::vector<Item> items;
			double total_value = 0.0;

			double temperature = config_.action_follow_temperature;
			if (temperature < 0.1) temperature = 0.1;

			node_->children_.ForEach([&](int choice, mcts::selection::EdgeAddon const* edge_addon, mcts::selection::TreeNode * child) {
				if (!CanBeChosen(choice)) return true;

				double choice_value = (double)edge_addon->GetChosenTimes();
				choice_value = pow(choice_value, 1.0 / temperature);
				total_value += choice_value;

				items.push_back({ choice_value, choice, child });
				return true;
			});

			// normalize
			double accumulated = 0.0;
			for (auto & item : items) {
				double normalized = item.value / total_value;
				accumulated += normalized;
				item.value = accumulated;
			}

			double v = std::uniform_real_distribution<double>(0.0, accumulated)(random);
			for (auto const& item : items) {
				if (v < item.value) {
					node_ = item.node;
					return item.choice;
				}
			}

			// if goes here, the only possible reason is we don't have any child nodes
			// no any choice is evaluated. randomly choose one.
			return random() % action_choices.Size();
		}

	private:
		MCTSAgentConfig config_;
		mcts::selection::TreeNode const* root_node_;
		mcts::selection::TreeNode const* node_;
		std::unique_ptr<MCTSRunner> controller_;
		AgentCallback cb_;
	};
}
