#pragma once

#include <unordered_map>
#include <memory>

#include "engine/view/Board.h"
#include "builder/TreeBuilder.h"
#include "MCTS/detail/BoardNodeMap.h"

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
			ActionParameterGetter(SOMCTS & callback) : callback_(callback) {}

			int GetNumber(engine::ActionType::Types action_type, engine::ActionChoices const& action_choices) final {
				if (action_type != engine::ActionType::kMainAction)
				{
					assert(action_choices.Size() > 0);
					if (action_choices.Size() == 1) return 0;
				}
				return callback_.ChooseAction(engine::ActionType(action_type), action_choices);
			}

		private:
			SOMCTS & callback_;
		};

	public:
		SOMCTS(builder::TreeBuilder::TreeNode & root, Statistic<> & statistic,
			std::mt19937 & selection_rand, std::mt19937 & simulation_rand) :
			turn_node_map_(nullptr), root_(root), statistic_(statistic),
			action_cb_(*this), builder_(action_cb_, statistic_, selection_rand, simulation_rand),
			node_(nullptr), stage_(Stage::kStageSelection), updater_()
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
				assert([](builder::TreeBuilder::TreeNode* node) {
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

			if (stage_ == kStageSimulation) {
				return builder_.PerformSimulate(board);
			}
			else {
				assert(stage_ == kStageSelection);
				assert(turn_node_map_);
				assert([](builder::TreeBuilder::TreeNode* node) {
					if (!node) return false;
					if (!node->GetActionType().IsValid()) return true;
					return node->GetActionType().GetType() == engine::ActionType::kMainAction;
				}(node_));
				auto perform_result = builder_.PerformSelect(node_, board, *turn_node_map_, &updater_);
				assert(perform_result.result != engine::kResultInvalid);

				assert([&](builder::TreeBuilder::TreeNode* node) {
					if (perform_result.result != engine::kResultNotDetermined) return true;
					if (!node) return false;
					if (!node->GetActionType().IsValid()) return true;
					return node->GetActionType().GetType() == engine::ActionType::kMainAction;
				}(perform_result.node));

				if (perform_result.result == engine::kResultNotDetermined) {
					if (perform_result.change_to_simulation) {
						stage_ = kStageSimulation;
						node_ = nullptr;
					}
					else {
						node_ = perform_result.node;
					}
				}

				return perform_result.result;
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
		
		int ChooseAction(engine::ActionType action_type, engine::ActionChoices const& choices) {
			if (stage_ == kStageSelection) {
				return builder_.ChooseSelectAction(action_type, choices);
			}
			else {
				assert(stage_ == kStageSimulation);
				return builder_.ChooseSimulateAction(action_type, choices);
			}
		}

		auto GetRootNode() const { return &root_; }

	private:
		builder::TreeBuilder::TreeNode & root_;
		Statistic<> & statistic_;

	private: // traversal progress
		detail::BoardNodeMap * turn_node_map_;
		ActionParameterGetter action_cb_;
		builder::TreeBuilder builder_;
		builder::TreeBuilder::TreeNode* node_;
		Stage stage_;
		builder::TreeUpdater updater_;
	};
}
