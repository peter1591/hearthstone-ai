#pragma once

#include "MCTS/selection/TreeNode.h"

namespace mcts
{
	namespace selection
	{
		class TraversedNodeInfo
		{
		public:
			TraversedNodeInfo(TreeNode* node) : node_(node), choice_(-1), edge_addon_(nullptr) {}

			TreeNode* GetNode() const { return node_; }
			int GetChoice() const { return choice_; }
			EdgeAddon* GetEdgeAddon() const { return edge_addon_; }

			bool HasMadeChoice() const { return choice_ >= 0; }

		public:
			void MakeChoice(int choice)
			{
				assert(!HasMadeChoice());
				choice_ = choice;
			}

			TreeNode* ConstructNextNode(bool * new_node_created)
			{
				assert(choice_ >= 0);
				auto result = node_->FollowChoice(choice_);
				edge_addon_ = &result.edge_addon;
				*new_node_created = result.just_expanded;
				return &result.node;
			}

			void ConstructRedirectNode()
			{
				// TODO: need to distinguish redirect node from invalid node
				bool dummy;
				ConstructNextNode(&dummy);
				// assert(choice_ >= 0);
				// sedge_addon_ = &node_->MarkChoiceInvalid(choice_);
			}

		private:
			TreeNode* node_;
			int choice_; // choice lead to next node
			EdgeAddon * edge_addon_;
		};
	}
}