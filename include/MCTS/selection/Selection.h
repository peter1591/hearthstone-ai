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
				// TODO: if random action is invalid, it means the last non-random action is invalid
				//    or, simply, a random action should not be an invalid action

				// a choose-from-card-ids should not be an invalid action, too.

				auto it = path_.rbegin();
				
				assert(it != path_.rend());
				TreeNode* child = it->node;
				int edge = it->leading_choice;

				++it;
				assert(it != path_.rend()); // the main action itself should always be valid
				TreeNode* parent = it->node;

				parent->MarkChildInvalid(edge, child);
			}

			std::vector<TraversedNodeInfo> const& GetTraversedPath() const { return path_; }

			TreeNode* GetCurrentNode() const { return path_.back().node; }

		private:
			std::pair<int, TreeNode*> SelectAction(board::Board const& board, ActionType action_type, board::ActionChoices const& choices, bool * new_node)
			{
				// TODO: use a addon to check ActionType
				
				if (action_type.IsChosenRandomly()) return SelectActionByRandom(choices);
				else return SelectActionByChoice(board, choices);
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

			std::pair<int, TreeNode*> SelectActionByRandom(board::ActionChoices const& choices)
			{
				return GetCurrentNode()->Select(choices, SelectRandomlyHelper());
			}

		private:
			std::pair<int, TreeNode*> SelectActionByChoice(board::Board const& board, board::ActionChoices const& choices)
			{
				using PolicyHelper = StaticConfigs::SelectionPhaseSelectActionPolicy;
				return GetCurrentNode()->Select(choices, PolicyHelper());
			}

		private:
			std::vector<TraversedNodeInfo> path_;

			std::vector<TraversedNodeInfo> saved_path_;
		};
	}
}
