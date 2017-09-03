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
				assert(result.node);
				return result.node;
			}

			void ConstructRedirectNode()
			{
				assert(choice_ >= 0);

				// The reason we need to construct a redirect node is:
				// we need edge addon to record the win-rate and so on...
				// Note that, from a specific node, it may leads to DIFFERENT redirect nodes
				// this happens, for example, when a spell hit a random target,
				// and since we intended to not construct a random node,
				// these different outcomes maps to different redirect nodes,
				// and these nodes are leads from a same node
				// In this sense, we do not need to know which redirect node it redirects to
				// only mark it as a redirect node, and use its edge addon
				edge_addon_ = &node_->MarkChoiceRedirect(choice_);
			}

			TreeNode* GetNextNode() {
				assert(choice_ >= 0);
				assert(node_);
				return node_->GetChildNode(choice_);
			}

		private:
			TreeNode* node_;
			int choice_; // choice lead to next node
			EdgeAddon * edge_addon_;
		};
	}
}