#pragma once

#include <unordered_map>
#include <memory>

#include "engine/view/Board.h"
#include "MCTS/Statistic.h"
#include "MCTS/builder/TreeUpdater.h"
#include "MCTS/detail/BoardNodeMap.h"
#include "MCTS/selection/Selection.h"
#include "MCTS/simulation/Simulation.h"

#include "MCTS/detail/BoardNodeMap-impl.h"

namespace mcts
{
	// single observer MCTS
	// Need to support the action 'ApplyOtherTurnAction'
	// Implementation details:
	//    Construct the game tree in a Monte-Carlo fashion
	//    To support the action 'ApplyOtherTurnAction',
	//       we use the SOMCTS_TreeNodeAddon to create a hash table for some tree noes
	//       so we can find a node corresponds to the given board, and step on it
	class SOMCTS
	{
	private:
		class ActionParameterGetter : public engine::IActionParameterGetter
		{
		public:
			ActionParameterGetter(SOMCTS & callback) : board_(nullptr), callback_(callback) {}

			void Initialize(engine::view::Board const& board) {
				board_ = &board;
				engine::IActionParameterGetter::Initialize(
					board_->GetCurrentPlayerStateRefView().GetValidActionGetter());
			}

			int GetNumber(engine::ActionType::Types action_type, engine::ActionChoices const& action_choices) final {
				assert(board_);
				if (action_type != engine::ActionType::kMainAction)
				{
					assert(action_choices.Size() > 0);
					if (action_choices.Size() == 1) return action_choices.Get(0);
				}
				return callback_.ChooseAction(*board_, engine::ActionType(action_type), action_choices);
			}

		private:
			engine::view::Board const* board_;
			SOMCTS & callback_;
		};

	public:
		SOMCTS(Statistic<> & statistic,
			std::mt19937 & selection_rand, std::mt19937 & simulation_rand) :
			statistic_(statistic), action_cb_(*this),
			stage_(Stage::kStageSelection), updater_(),
			selection_stage_(selection_rand), simulation_stage_(simulation_rand)
		{}

		SOMCTS(SOMCTS const&) = delete;
		SOMCTS & operator=(SOMCTS const&) = delete;

		void StartEpisode(selection::TreeNode * root)
		{
			selection_stage_.SetRootNode(root);
			stage_ = kStageSelection;
			updater_.Clear();
		}

		void StartActions() {
			if (stage_ == kStageSelection) {
				selection_stage_.StartNewMainActions();
			}
		}

		engine::Result PerformAction(engine::view::Board const& board)
		{
			assert(board.GetCurrentPlayer().GetSide() == board.GetViewSide());

			action_cb_.Initialize(board);

			if (stage_ == kStageSimulation) {
				engine::Result result = simulation_stage_.CutoffCheck(board);
				assert(result != engine::kResultInvalid);
				if (result != engine::kResultNotDetermined) return result;

				result = board.ApplyAction(action_cb_);
				assert(result != engine::kResultInvalid);

				constexpr bool is_simulation = true;
				statistic_.ApplyActionSucceeded(is_simulation);
				return result;
			}
			else {
				assert(stage_ == kStageSelection);

				selection_stage_.StartNewMainAction(board);

				auto result = board.ApplyAction(action_cb_);
				assert(result != engine::kResultInvalid);

				selection_stage_.FinishMainAction(board, result);

				constexpr bool is_simulation = false;
				statistic_.ApplyActionSucceeded(is_simulation);

				assert(result != engine::kResultInvalid);

				if (result == engine::kResultNotDetermined) {
					if (selection_stage_.HasNewNodeCreated()) {
						stage_ = kStageSimulation;
					}
					else if (selection_stage_.GetTraversedPath().back().GetEdgeAddon()->GetChosenTimes() < StaticConfigs::kSwitchToSimulationUnderChosenTimes) {
						stage_ = kStageSimulation;
					}
				}

				updater_.PushBackNodes(selection_stage_.GetMutableTraversedPath(), selection_stage_.GetNode());
				return result;
			}
		}

		// Another player finished his actions
		// Just jump to the node present the new state
		void ApplyOthersActions(engine::view::Board const& board)
		{
			if (stage_ == kStageSimulation) return;

			assert(stage_ == kStageSelection);

			selection_stage_.JumpToBoardNode(board);
		}

		void EpisodeFinished(engine::view::Board const& board, engine::Result result)
		{
			double credit = mcts::StaticConfigs::CreditPolicy::GetCredit(board, result);
			assert(credit >= 0.0);
			assert(credit <= 1.0); // TODO: should take into account episilon precision
			updater_.Update(credit);
		}
		
		int ChooseAction(engine::view::Board const& board, engine::ActionType action_type, engine::ActionChoices const& choices) {
			assert(!choices.Empty());

			if (stage_ == kStageSelection) {
				int choice = selection_stage_.ChooseAction(board, action_type, choices);
				assert(choice >= 0); // always return a valid choice
				return choice;
			}
			else {
				assert(stage_ == kStageSimulation);
				int choice = simulation_stage_.ChooseAction(
					board,
					action_cb_.GetAnalyzer(),
					action_type, choices);
				assert(choice >= 0);
				return choice;
			}
		}

	private:
		Statistic<> & statistic_;

	private: // traversal progress
		ActionParameterGetter action_cb_;
		Stage stage_;
		builder::TreeUpdater updater_;
		selection::Selection selection_stage_;
		simulation::Simulation simulation_stage_;
	};
}
