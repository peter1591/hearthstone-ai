#pragma once

#include <unordered_map>
#include <memory>

#include "MCTS/policy/CreditPolicy.h"
#include "MCTS/policy/RandomByRand.h"
#include "MCTS/policy/Selection.h"
#include "MCTS/policy/SideController.h"
#include "MCTS/policy/Simulation.h"
#include "MCTS/policy/StageController.h"


#include "engine/view/Board.h"
#include "MCTS/Statistic.h"
#include "MCTS/selection/Selection.h"
#include "MCTS/simulation/Simulation.h"

#include "MCTS/selection/BoardNodeMap-impl.h"

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
		enum Stage {
			kStageSelection,
			kStageSimulation
		};

		class ActionParameterGetter : public engine::IActionParameterGetter
		{
		public:
			ActionParameterGetter(SOMCTS & callback) : board_(nullptr), callback_(callback) {}

			ActionParameterGetter(ActionParameterGetter const&) = delete;
			ActionParameterGetter & operator=(ActionParameterGetter const&) = delete;

			void Initialize(engine::view::Board const& board) {
				board_ = &board;
				engine::IActionParameterGetter::Initialize(
					board_->GetCurrentPlayerStateRefView().GetValidActionGetter());
			}

			int GetNumber(engine::ActionType::Types action_type, engine::ActionChoices & action_choices) final {
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
		SOMCTS(selection::TreeNode & tree, Statistic<> & statistic, 
			std::mt19937 & selection_rand, std::mt19937 & simulation_rand,
			Config const& config) :
			action_cb_(*this), stage_(Stage::kStageSelection),
			selection_stage_(tree, selection_rand, config), simulation_stage_(simulation_rand, config),
			statistic_(statistic)
		{}

		SOMCTS(SOMCTS const&) = delete;
		SOMCTS & operator=(SOMCTS const&) = delete;
		
		auto GetRootNode() const { return selection_stage_.GetRootNode(); }

		void StartIteration()
		{
			selection_stage_.StartIteration();
			stage_ = kStageSelection;
		}

		void StartActions() {
		}

		// return true if iteration should end (early cutoff, or game ends)
		bool PerformAction(engine::view::Board const& board, StateValue & state_value)
		{
			assert(board.GetCurrentPlayer().GetSide() == board.GetViewSide());

			action_cb_.Initialize(board);

			engine::Result result;
			if (stage_ == kStageSimulation) {
				if (simulation_stage_.CutoffCheck(board, state_value)) return true;
				
				simulation_stage_.StartAction(board, action_cb_.GetAnalyzer());

				result = board.ApplyAction(action_cb_);
				assert(result != engine::kResultInvalid);
				constexpr bool is_simulation = true;
				statistic_.ApplyActionSucceeded(is_simulation);
			}
			else {
				assert(stage_ == kStageSelection);

				selection_stage_.StartAction(board);

				result = board.ApplyAction(action_cb_);
				assert(result != engine::kResultInvalid);
				constexpr bool is_simulation = false;
				statistic_.ApplyActionSucceeded(is_simulation);

				if (selection_stage_.FinishAction(board, result)) {
					stage_ = kStageSimulation;
				}
			}

			if (result != engine::kResultNotDetermined) {
				state_value.SetValue(result);
				return true;
			}
			else {
				return false;
			}
		}

		// Another player finished his actions
		// Just jump to the node present the new state
		void ApplyOthersActions(engine::view::Board const& board)
		{
			if (stage_ == kStageSimulation) return;

			assert(stage_ == kStageSelection);
			selection_stage_.ApplyOthersActions(board);
		}

		void FinishIteration(engine::view::Board const& board, StateValue state_value)
		{
			selection_stage_.FinishIteration(board, state_value);
		}
		
		int ChooseAction(engine::view::Board const& board, engine::ActionType action_type, engine::ActionChoices & choices) {
			assert(!choices.Empty());

			if (stage_ == kStageSelection) {
				int choice = selection_stage_.ChooseAction(action_type, choices);
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
		ActionParameterGetter action_cb_;
		Stage stage_;
		selection::Selection selection_stage_;
		simulation::Simulation simulation_stage_;
		Statistic<> & statistic_;
	};
}
