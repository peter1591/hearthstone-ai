#pragma once

#include <unordered_map>
#include <memory>

#include "MCTS/Types.h"
#include "MCTS/detail/TreeNodeBase.h"
#include "MCTS/selection/TreeNodeAddon.h"
#include "MCTS/board/ActionChoices.h"

namespace mcts
{
	namespace selection
	{
		class TreeNode
		{
		public:
			using Edge = int;

			TreeNode() : action_type_(ActionType::kInvalid),
				choices_type_(board::ActionChoices::kInvalid)
			{}

			// select among specific choices
			// if any of the choices does not exist, return the edge to expand it
			//    A new node will be allocated
			// if all of the specific exist, use select_callback to select one of them
			//    Call select_callback.ReportChoicesCount(int count)
			//    Call select_callback.AddChoice(int choice, TreeNode* node) for each choices
			//        If an action is marked invalid before, a nullptr is passed to the 'node' parameter
			//    Call select_callback.SelectChoice() -> std::pair<Edge,TreeNode*> to get result
			// Return { -1, nullptr } if all choices are invalid.
			template <typename SelectCallback>
			std::pair<Edge,TreeNode*>
			Select(ActionType action_type, board::ActionChoices choices, SelectCallback && select_callback)
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

				choices.Begin();
				while (!choices.IsEnd()) {
					Edge choice = choices.Get();

					auto it = children_.find(choice);
					if (it == children_.end()) {
						TreeNode* new_node = new TreeNode();
						children_.insert({ choice, std::unique_ptr<TreeNode>(new_node) });
						return { choice, new_node };
					}

					select_callback.AddChoice(choice, it->second.get());
					choices.StepNext();
				}

				return select_callback.SelectChoice();
			}

			void MarkChildInvalid(Edge edge, TreeNode* child)
			{
				auto it = children_.find(edge);
				assert(it != children_.end());
				if (!it->second) return;
				assert(it->second.get() == child);
				it->second.reset();
			}

		public:
			ActionType GetActionType() const { return action_type_; }

		public:
			TreeNodeAddon const& GetAddon() const { return addon_; }
			TreeNodeAddon & GetAddon() { return addon_; }

		private:
			ActionType action_type_;
			board::ActionChoices::Type choices_type_;
			std::unordered_map<Edge, std::unique_ptr<TreeNode>> children_;
			TreeNodeAddon addon_;
		};
	}
}