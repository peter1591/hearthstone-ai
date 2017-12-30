#pragma once

#include <assert.h>
#include "MCTS/Config.h"
#include "MCTS/selection/TreeNode.h"
#include "MCTS/selection/TraversedNodeInfo.h"
#include "MCTS/builder/TreeUpdater.h"

#include "MCTS/selection/ChildNodeMap-impl.h"

namespace mcts
{
	namespace selection
	{
		class Selection
		{
		public:
			Selection(TreeNode & tree, std::mt19937 & rand) :
				root_(tree), node_(nullptr), turn_node_map_(nullptr), path_(), random_(rand), policy_(),
				new_node_created_(false), pending_randoms_(false), updater_()
			{}

			Selection(Selection const&) = delete;
			Selection & operator=(Selection const&) = delete;

			auto GetRootNode() const { return &root_; }

			TreeNode* GetNode() const { return node_; }
			
			void StartIteration() {
				node_ = &root_;
				updater_.Clear();
			}

			void StartActions() {
				assert(node_);
				assert([](selection::TreeNode * node) {
					if (!node) return false;
					if (!node->GetActionType().IsValid()) return true;
					return node->GetActionType().GetType() == engine::ActionType::kMainAction;
				}(node_));
				turn_node_map_ = &node_->GetAddon().board_node_map;
			}

			void StartAction(engine::view::Board const& board) {
				assert(node_);
				assert([&]() {
					if (!node_->GetActionType().IsValid()) return true;
					return node_->GetActionType().GetType() == engine::ActionType::kMainAction;
				}());
				(void)board;
				assert(node_->GetAddon().consistency_checker.CheckBoard(board.CreateView()));
				
				assert(turn_node_map_);

				path_.clear();
				path_.emplace_back(node_);
				new_node_created_ = false;
				pending_randoms_ = false;
			}

			// @return >= 0 for the chosen action
			int ChooseAction(
				engine::view::Board const& board,
				engine::ActionType action_type,
				engine::ActionChoices const& choices)
			{
				assert(!choices.Empty());

				if (action_type.IsChosenRandomly()) {
					assert(choices.CheckType<engine::ActionChoices::ChooseFromZeroToExclusiveMax>());
					pending_randoms_ = true;
					return random_.GetRandom(choices.Size());
				}

				assert(action_type.IsChosenManually());
				assert(!path_.empty());
				if (path_.back().HasMadeChoice()) {
					TreeNode* new_node = path_.back().ConstructNextNode(&new_node_created_);
					assert(new_node);
					path_.emplace_back(new_node);
				}

				assert(!path_.back().HasMadeChoice());
				TreeNode* current_node = path_.back().GetNode();

				if (pending_randoms_) {
					switch (action_type.GetType()) {
					case engine::ActionType::kChooseOne:
						// choose-one card might be triggered after random
						break;

					default:
						// Not allow random actions to be conducted first before any sub-action
						// That is, the player
						//    1. Choose the hand card
						//    2. Choose minion put location
						// Then, random gets started
						// Similar to other sub-actions
						assert(false);
						throw std::runtime_error("not allow random to be conducted before any sub-action.");
					}

					// Check if the board-view and sub-action-type are consistency
					// Currently, we have an assumption that
					//    * A random action can be swapped to the end of a sub-action sequence
					//      without any distinguishable difference
					// But, it may not be this case if...
					//    1. You have a 50% chance to choose a card
					//    2. Summon a random minion, then choose a card
					// In both cases, a random node should be added before the sub-action 'choose-a-card'
					// From a game-play view, that is to say
					//    The random outcome *influences* the player's decision
					// So, if this assertion failed
					//    It means a random node is necessary before this node
					assert(current_node->GetAddon().consistency_checker.SetAndCheck(board, action_type, choices));
				}
				else {
					assert(current_node->GetAddon().consistency_checker.SetAndCheck(board, action_type, choices));
				}

				int next_choice = current_node->Select(action_type, choices, policy_);
				assert(next_choice >= 0); // should report a valid action

				path_.back().MakeChoice(next_choice);

				return next_choice;
			}

			// @return  If we should switch to simulation stage
			bool FinishAction(engine::view::Board const& board, engine::Result result) {
				// mark the last action as a redirect node
				path_.back().ConstructRedirectNode();

				if (result != engine::kResultNotDetermined) {
					node_ = nullptr;
					updater_.PushBackNodes(path_, node_);
					return false;
				}

				bool new_node_created = false;
				node_ = turn_node_map_->GetOrCreateNode(board, &new_node_created);
				assert(node_);
				assert([&](selection::TreeNode* node) {
					if (!node->GetActionType().IsValid()) return true; // TODO: should not in this case?
					return node->GetActionType().GetType() == engine::ActionType::kMainAction;
				}(node_));

				if (new_node_created) new_node_created_ = true;

				bool switch_to_simulation = StaticConfigs::StageController::SwitchToSimulation(
					new_node_created_, path_.back().GetEdgeAddon()->GetChosenTimes());

				updater_.PushBackNodes(path_, node_);

				return switch_to_simulation;
			}

			void JumpToNodeWithBoard(engine::view::Board const& board) {
				assert(node_);
				node_ = node_->GetAddon().board_node_map.GetOrCreateNode(board);
			}

			void FinishIteration(engine::view::Board const& board, engine::Result result)
			{
				double credit = StaticConfigs::CreditPolicy::GetCredit(board, result);
				assert(credit >= 0.0);
				assert(credit <= 1.0); // TODO: should take into account episilon precision
				updater_.Update(credit);
			}

		private:
			selection::TreeNode & root_;
			TreeNode * node_;
			detail::BoardNodeMap * turn_node_map_;
			std::vector<TraversedNodeInfo> path_;
			StaticConfigs::SelectionPhaseRandomActionPolicy random_;
			StaticConfigs::SelectionPhaseSelectActionPolicy policy_;
			bool new_node_created_;
			bool pending_randoms_;
			builder::TreeUpdater updater_;
		};
	}
}
