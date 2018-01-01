#pragma once

#include "MCTS/selection/TreeNode.h"

namespace mcts
{
	namespace selection
	{
		struct TraversedNodeInfo {
			TreeNode* node_;
			int choice_; // choice lead to next node
			EdgeAddon * edge_addon_;

			TraversedNodeInfo(TreeNode* node, int choice, EdgeAddon * edge_addon) :
				node_(node), choice_(choice), edge_addon_(edge_addon)
			{}
		};

		class TraversedNodesInfo {
		public:
			TraversedNodesInfo() : path_(), new_node_created_(false), pending_choice_(-1) {}

			void Restart(TreeNode * node) {
				path_.clear();
				current_node_ = node;
				assert(current_node_);
				new_node_created_ = false;
				pending_choice_ = -1;
			}

			void ConstructNode() {
				if (pending_choice_ >= 0) {
					TreeNode* new_node = AddNodeToPath();
					assert(new_node);
					current_node_ = new_node;
					pending_choice_ = -1;
				}
			}

			void ConstructRedirectNode() {
				// The reason we need to construct a redirect node is:
				// we need edge addon to record the win-rate and so on...
				// Note that, from a specific node, it may leads to DIFFERENT redirect nodes
				// this happens, for example, when a spell hit a random target,
				// and since we intended to not construct a random node,
				// these different outcomes maps to different redirect nodes,
				// and these nodes are leads from a same node
				// In this sense, we do not need to know which redirect node it redirects to
				// only mark it as a redirect node, and use its edge addon
				AddRedirectNodeToPath();
			}

			TreeNode * GetCurrentNode() const {
				return current_node_;
			}

			void MakeChoiceForCurrentNode(int choice) {
				pending_choice_ = choice;
			}

			bool HasCurrentNodeMadeChoice() const {
				return pending_choice_ >= 0;
			}

			auto & GetMutablePath() { return path_; }
			auto const& GetPath() const { return path_; }

			bool HasNewNodeCreated() const { return new_node_created_; }

		private:
			TreeNode* AddNodeToPath() {
				assert(current_node_);
				assert(pending_choice_ >= 0);
				auto result = current_node_->FollowChoice(pending_choice_);
				path_.emplace_back(current_node_, pending_choice_, &result.edge_addon);
				new_node_created_ = result.just_expanded;
				return result.node;
			}

			void AddRedirectNodeToPath() {
				assert(current_node_);
				assert(pending_choice_ >= 0);
				auto & edge_addon = current_node_->MarkChoiceRedirect(pending_choice_);
				path_.emplace_back(current_node_, pending_choice_, &edge_addon);
			}

		private:
			std::vector<TraversedNodeInfo> path_;
			bool new_node_created_;
			TreeNode * current_node_;
			int pending_choice_;
		};
	}
}