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

		class TraversedNodesInfo {
		public:
			TraversedNodesInfo() : path_(), new_node_created_(false) {}

			bool Empty() const { return path_.empty(); }

			void Restart(TreeNode * node) {
				path_.clear();
				path_.emplace_back(node);
				new_node_created_ = false;
			}

			void ConstructNodeForCurrentNode() {
				if (path_.back().HasMadeChoice()) {
					TreeNode* new_node = path_.back().ConstructNextNode(&new_node_created_);
					assert(new_node);
					path_.emplace_back(new_node);
				}
			}

			void ConstructRedirectNodeForCurrentNode() {
				path_.back().ConstructRedirectNode();
			}

			bool HasCurrentNodeMadeChoice() const {
				return path_.back().HasMadeChoice();
			}

			TreeNode * GetCurrentNode() const {
				return path_.back().GetNode();
			}

			void MakeChoiceForCurrentNode(int choice) {
				path_.back().MakeChoice(choice);
			}

			auto & GetMutablePath() { return path_; }
			auto const& GetPath() const { return path_; }

			bool HasNewNodeCreated() const { return new_node_created_; }

		private:
			std::vector<TraversedNodeInfo> path_;
			bool new_node_created_;
		};
	}
}