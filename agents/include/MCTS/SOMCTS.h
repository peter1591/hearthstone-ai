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
	//    Use builder::TreeBuilder to construct the game tree in a Monte-Carlo fashion
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
					if (action_choices.Size() == 1) return 0;
				}
				return callback_.ChooseAction(*board_, engine::ActionType(action_type), action_choices);
			}

		private:
			engine::view::Board const* board_;
			SOMCTS & callback_;
		};

	public:
		SOMCTS(selection::TreeNode & root, Statistic<> & statistic,
			std::mt19937 & selection_rand, std::mt19937 & simulation_rand) :
			turn_node_map_(nullptr), root_(root), statistic_(statistic),
			action_cb_(*this),
			node_(nullptr), stage_(Stage::kStageSelection), updater_(),
			selection_stage_(selection_rand), simulation_stage_(simulation_rand)
		{}

		SOMCTS(SOMCTS const&) = delete;
		SOMCTS & operator=(SOMCTS const&) = delete;

		void StartEpisode()
		{
			node_ = &root_;
			stage_ = kStageSelection;
			updater_.Clear();
		}

		void StartActions() {
			if (stage_ == kStageSelection) {
				assert(node_);
				assert([](selection::TreeNode * node) {
					if (!node) return false;
					if (!node->GetActionType().IsValid()) return true;
					return node->GetActionType().GetType() == engine::ActionType::kMainAction;
				}(node_));
				turn_node_map_ = &node_->GetAddon().board_node_map;
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
				assert(turn_node_map_);
				assert(node_);
				assert([](selection::TreeNode* node) {
					if (!node->GetActionType().IsValid()) return true;
					return node->GetActionType().GetType() == engine::ActionType::kMainAction;
				}(node_));
				assert(node_->GetAddon().consistency_checker.CheckBoard(board.CreateView()));

				selection_stage_.StartNewMainAction(node_);

				auto result = board.ApplyAction(action_cb_);
				assert(result != engine::kResultInvalid);

				constexpr bool is_simulation = false;
				statistic_.ApplyActionSucceeded(is_simulation);

				selection::TreeNode * next_node = nullptr;
				assert(result != engine::kResultInvalid);

				// we use mutable here, since we will throw it away after all
				auto & traversed_path = selection_stage_.GetMutableTraversedPath();

				// mark the last action as a redirect node
				traversed_path.back().ConstructRedirectNode();

				bool new_node_created = false;
				if (result == engine::kResultNotDetermined) {
					next_node = turn_node_map_->GetOrCreateNode(board, &new_node_created);
					assert(next_node);
					assert([&](selection::TreeNode* node) {
						if (!node->GetActionType().IsValid()) return true; // TODO: should not in this case?
						return node->GetActionType().GetType() == engine::ActionType::kMainAction;
					}(next_node));
				}
				else {
					if (result == engine::kResultFirstPlayerWin) {
						next_node = mcts::selection::TreeNode::GetFirstPlayerWinNode();
					}
					else if (result == engine::kResultDraw) {
						next_node = mcts::selection::TreeNode::GetDrawNode();
					}
					else {
						assert(result == engine::kResultSecondPlayerWin);
						next_node = mcts::selection::TreeNode::GetSecondPlayerWinNode();
					}
					assert(next_node != nullptr);
				}

				if (!new_node_created) {
					new_node_created = selection_stage_.HasNewNodeCreated();
				}

				bool change_to_simulation = false;
				if (new_node_created) {
					change_to_simulation = true;
				}
				else if (traversed_path.back().GetEdgeAddon()->GetChosenTimes() < StaticConfigs::kSwitchToSimulationUnderChosenTimes) {
					change_to_simulation = true;
				}

				updater_.PushBackNodes(traversed_path, next_node);

				if (result == engine::kResultNotDetermined) {
					if (change_to_simulation) {
						stage_ = kStageSimulation;
						node_ = nullptr;
					}
					else {
						node_ = next_node;
					}
				}

				return result;
			}
		}

		// Another player finished his actions
		// Just jump to the node present the new state
		void ApplyOthersActions(engine::view::Board const& board)
		{
			if (stage_ == kStageSimulation) return;

			assert(stage_ == kStageSelection);
			assert(node_);

			node_ = node_->GetAddon().board_node_map.GetOrCreateNode(board);
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

		auto GetRootNode() const { return &root_; }

	private:
		selection::TreeNode & root_;
		Statistic<> & statistic_;

	private: // traversal progress
		detail::BoardNodeMap * turn_node_map_;
		ActionParameterGetter action_cb_;
		selection::TreeNode * node_;
		Stage stage_;
		builder::TreeUpdater updater_;
		selection::Selection selection_stage_;
		simulation::Simulation simulation_stage_;
	};
}
