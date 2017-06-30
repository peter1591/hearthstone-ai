#pragma once

#include <unordered_map>
#include <memory>

#include "MCTS/Types.h"
#include "MCTS/detail/TreeNodeBase.h"
#include "MCTS/selection/TreeNodeAddon.h"
#include "MCTS/board/ActionChoices.h"
#include "MCTS/selection/EdgeAddon.h"
#include "MCTS/selection/ChildNodeMap.h"

namespace mcts
{
	namespace selection
	{
		class TreeNode
		{
		public:
			TreeNode() : action_type_(ActionType::kInvalid),
				choices_type_(board::ActionChoices::kInvalid)
			{}

			// select among specific choices
			// if any of the choices does not exist, return the edge to expand it
			//    A new node will be allocated
			// if all of the specific exist, use select_callback to select one of them
			//    Call select_callback.ReportChoicesCount(int count)
			//        Include invalid choices
			//    Call select_callback.AddChoice(int choice, EdgeAddon, TreeNode* node) for each choices
			//        If an action is marked invalid before, a nullptr is passed to the 'node' parameter
			//    Call select_callback.SelectChoice() -> TreeNode to get result
			// Return -1 if all choices are invalid.
			template <typename SelectCallback>
			int Select(ActionType action_type, board::ActionChoices choices, SelectCallback && select_callback)
			{
				if (choices_type_ == board::ActionChoices::kInvalid) {
					choices_type_ = choices.GetType();
				}
				else {
					assert(choices_type_ == choices.GetType());
				}

				if (action_type_.IsValid()) {
					assert(action_type_ == action_type);
				}
				else {
					action_type_ = action_type;
				}

				select_callback.ReportChoicesCount(choices.Size());

				for (choices.Begin(); !choices.IsEnd(); choices.StepNext()) {
					int choice = choices.Get();

					auto child = children_.Get(choice);
					if (!child) return choice;

					if (!child->node) continue; // invalid choice
					if (child->edge_addon.chosen_times <= 0) return choice; // not yet chosen
					select_callback.AddChoice(choice, child->edge_addon, child->node.get());
				}

				return select_callback.SelectChoice();
			}

			// @return  (pointer to child, is_just_expanded)
			struct FollowStatus {
				bool just_expanded;
				EdgeAddon & edge_addon;
				TreeNode & node;
			};
			FollowStatus FollowChoice(int choice)
			{
				ChildType* child = children_.Get(choice);
				bool just_expanded = false;
				if (!child) {
					child = children_.Create(choice);
					child->node.reset(new TreeNode());
					just_expanded = true;
				}

				TreeNode* child_node = child->node.get();
				assert(child_node); // should only follow valid choices
				return { just_expanded, child->edge_addon, *child_node };
			}

			void MarkChoiceInvalid(int choice, TreeNode* child_node)
			{
				if (!child_node) {
					// the child node is not yet created
					// since we delay the node creation as late as possible
					auto child = children_.Get(choice);
					if (child) {
						assert(!child->node);
						return;
					}
					children_.Create(choice);
					return;
				}

				auto child = children_.Get(choice);
				assert(child);
				if (!child->node) return;
				
				assert(child->node.get() == child_node);
				child->node.reset();
			}

		public:
			ActionType GetActionType() const { return action_type_; }

		public:
			TreeNodeAddon const& GetAddon() const { return addon_; }
			TreeNodeAddon & GetAddon() { return addon_; }

		private:
			ActionType action_type_;
			board::ActionChoices::Type choices_type_; // TODO: debug only

			ChildNodeMap children_;
			TreeNodeAddon addon_;
		};
	}
}