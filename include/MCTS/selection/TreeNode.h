#pragma once

#include <unordered_map>
#include <memory>

#include "MCTS/Types.h"
#include "MCTS/detail/TreeNodeBase.h"
#include "MCTS/selection/TreeNodeAddon.h"
#include "MCTS/board/ActionChoices.h"
#include "MCTS/selection/EdgeAddon.h"

namespace mcts
{
	namespace selection
	{
		class TreeNode
		{
		public:
			using ChildType = std::pair<EdgeAddon, std::unique_ptr<TreeNode>>;
			using ChildMapType = std::unordered_map<int, ChildType>;

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

					auto it = children_.find(choice);
					if (it == children_.end()) return choice;

					auto const& edge_addon = it->second.first;
					auto const& child_node = it->second.second;

					if (!child_node) continue; // invalid choice
					if (edge_addon.chosen_times <= 0) return choice; // not yet chosen
					select_callback.AddChoice(choice, edge_addon, child_node.get());
				}

				return select_callback.SelectChoice();
			}

			std::pair<ChildMapType::iterator, bool> GetOrCreateChild(int choice)
			{
				auto it = children_.find(choice);
				if (it != children_.end()) return { it, false };
				
				TreeNode* new_node = new TreeNode();
				return children_.insert({ choice,
					std::make_pair(EdgeAddon(), std::unique_ptr<TreeNode>(new_node))
				});
			}

			void MarkChildInvalid(int edge, TreeNode* child)
			{
				if (!child) {
					// the child node is not yet created
					// since we delay the node creation as late as possible
					auto it = children_.find(edge);
					if (it != children_.end()) {
						assert(!it->second.second);
						return;
					}
					children_.insert({ edge, std::make_pair(EdgeAddon(), nullptr)});
					return;
				}

				auto it = children_.find(edge);
				assert(it != children_.end());
				if (!it->second.second) return;
				assert(it->second.second.get() == child);
				it->second.second.reset();
			}

		public:
			ActionType GetActionType() const { return action_type_; }

		public:
			TreeNodeAddon const& GetAddon() const { return addon_; }
			TreeNodeAddon & GetAddon() { return addon_; }

		private:
			ActionType action_type_;
			board::ActionChoices::Type choices_type_;

			ChildMapType children_;
			TreeNodeAddon addon_;
		};
	}
}