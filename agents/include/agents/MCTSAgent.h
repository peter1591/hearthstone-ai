#pragma once

#include <atomic>
#include <chrono>
#include <thread>

#include "state/State.h"
#include "MCTS/MOMCTS.h"
#include "judge/IAgent.h"
#include "judge/Judger.h"
#include "agents/MCTSRunner.h"

namespace agents
{
	class MCTSAgent : public judge::IAgent {
	public:
		MCTSAgent(int threads, int tree_samples) :
			threads_(threads),
			tree_samples_(tree_samples),
			root_node_(nullptr), node_(nullptr), controller_()
		{}

		MCTSAgent(MCTSAgent const&) = delete;
		MCTSAgent & operator=(MCTSAgent const&) = delete;

		void Think(state::State const& state, int seed, std::function<bool(uint64_t)> cb) {
			auto continue_checker = [&]() {
				uint64_t iterations = controller_->GetStatistic().GetSuccededIterates();
				return cb(iterations);
			};

			std::mt19937 rand(seed);
			controller_.reset(new MCTSRunner(tree_samples_, rand));
			controller_->Run(threads_, rand(), [&](int seed) {
				(void)seed;
				return state;
			});

			while (true) {
				if (!continue_checker()) break;
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
			controller_->WaitUntilStopped();

			node_ = controller_->GetRootNode(state.GetCurrentPlayerId());
			root_node_ = node_;
		}

		int GetMainAction() final {
			assert(root_node_ == node_);
			assert(node_);
			assert(node_->GetActionType().GetType() == mcts::ActionType::kMainAction);

			int best_choice = -1;
			double best_chosen_times = -std::numeric_limits<double>::infinity();
			mcts::builder::TreeBuilder::TreeNode const* best_node = nullptr;
			root_node_->ForEachChild([&](int choice, mcts::selection::ChildType const& child) {
				double chosen_times = (double)child.GetEdgeAddon().GetChosenTimes();
				if (chosen_times > best_chosen_times) {
					best_chosen_times = chosen_times;
					best_choice = choice;
					best_node = child.GetNode();
				}
				return true;
			});

			node_ = best_node;

			return best_choice;
		}

		mcts::board::BoardActionAnalyzer GetActionApplier() final {
			return root_node_->GetAddon().action_analyzer;
		}

		int GetSubAction(mcts::ActionType::Types action_type, mcts::board::ActionChoices action_choices) final {
			assert(node_);

			if (node_->GetActionType() != action_type) {
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
			mcts::builder::TreeBuilder::TreeNode const* best_node = nullptr;
			node_->ForEachChild([&](int choice, mcts::selection::ChildType const& child) {
				if (!CanBeChosen(choice)) return true;

				double chosen_times = (double)child.GetEdgeAddon().GetChosenTimes();
				if (chosen_times > best_chosen_times) {
					best_chosen_times = chosen_times;
					best_choice = choice;
					best_node = child.GetNode();
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
		int threads_;
		int tree_samples_;
		mcts::builder::TreeBuilder::TreeNode const* root_node_;
		mcts::builder::TreeBuilder::TreeNode const* node_;
		std::unique_ptr<MCTSRunner> controller_;
	};
}