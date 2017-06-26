#pragma once

#include <assert.h>
#include "MCTS/Config.h"
#include "MCTS/selection/TreeNode.h"
#include "MCTS/selection/TraversedNodeInfo.h"

namespace mcts
{
	namespace selection
	{
		class Selection
		{
		public:
			void StartNewAction() { saved_path_ = path_; }
			void RestartAction() { path_ = saved_path_; }

			void StartEpisode(TreeNode * root)
			{
				path_.clear();
				StepNext(-1, root);
			}

			// @return >= 0 for the chosen action; < 0 if no valid action
			int GetAction(board::Board const& board, ActionType action_type, board::ActionChoices const& choices, bool * created_new_node)
			{
				auto next_info = SelectAction(board, action_type, choices, created_new_node);
				
				int next_choice = next_info.first;
				TreeNode* next_node = next_info.second;

				if (!next_node) {
					// all of the choices are invalid actions
					return -1;
				}

				StepNext(next_choice, next_node);
				return next_choice;
			}

			void ReportInvalidAction() {
				auto it = path_.rbegin();
				
				assert(it != path_.rend());
				TreeNode* child = it->node;
				TreeNode* parent = nullptr;
				int edge = it->leading_choice;

				while (true) {
					++it;
					assert(it != path_.rend()); // we should be able to find a blame node along the path
					parent = it->node;

					// if a sub-action failed, it means the main action failed.
					//    More precisely, it means the calling to FlowController is failed
					//    not only the callback like 'GetTarget()' or 'ChooseOne()' is failed
					//    so we find the node to blame, and remove it from its parent
					if (parent->GetActionType().IsInvalidStateBlameNode()) break;

					// look up further to find the blame node
					child = it->node;
					edge = it->leading_choice;
				}

				parent->MarkChildInvalid(edge, child);
			}

			std::vector<TraversedNodeInfo> const& GetTraversedPath() const { return path_; }

			TreeNode* GetCurrentNode() const { return path_.back().node; }

		private:
			std::pair<int, TreeNode*> SelectAction(board::Board const& board, ActionType action_type, board::ActionChoices const& choices, bool * new_node)
			{
				// TODO: use a addon to check ActionType
				
				if (action_type.IsChosenRandomly()) return SelectActionByRandom(action_type, choices);
				else return SelectActionByChoice(action_type, board, choices);
			}

			void StepNext(int leading_choice, TreeNode* next_node)
			{
				path_.push_back({ leading_choice, next_node });
			}

		private:
			class SelectRandomlyHelper {
			public:
				SelectRandomlyHelper() : idx_(0), target_idx_(0), result_{ 0, nullptr } {}

				void ReportChoicesCount(int count) {
					target_idx_ = StaticConfigs::SelectionPhaseRandomActionPolicy::GetRandom(count);
					idx_ = 0;
				}
				void AddChoice(int choice, TreeNode* node) {
					// if an action was reported as invalid before, a nullptr is passed to 'node'
					assert(node); // random actions should be an invalid action
					if (idx_ == target_idx_) result_ = { choice, node };
					++idx_;
				}
				std::pair<int, TreeNode*> SelectChoice() {
					assert(result_.second);
					return result_;
				}

			private:
				int idx_;
				int target_idx_;
				std::pair<int, TreeNode*> result_;
			};

			std::pair<int, TreeNode*> SelectActionByRandom(ActionType action_type, board::ActionChoices const& choices)
			{
				return GetCurrentNode()->Select(action_type, choices, SelectRandomlyHelper());
			}

		private:
			std::pair<int, TreeNode*> SelectActionByChoice(ActionType action_type, board::Board const& board, board::ActionChoices const& choices)
			{
				using PolicyHelper = StaticConfigs::SelectionPhaseSelectActionPolicy;
				return GetCurrentNode()->Select(action_type, choices, PolicyHelper());
			}

		private:
			std::vector<TraversedNodeInfo> path_;

			std::vector<TraversedNodeInfo> saved_path_;
		};
	}
}
