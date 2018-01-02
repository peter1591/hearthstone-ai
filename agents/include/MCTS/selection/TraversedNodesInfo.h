#pragma once

#include "MCTS/selection/TraversedNodeInfo.h"
#include "MCTS/selection/TreeUpdater.h"

namespace mcts {
	namespace selection {
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

			TreeNode * GetCurrentNode() const {
				return current_node_;
			}

			void MakeChoiceForCurrentNode(int choice) {
				pending_choice_ = choice;
			}

			bool HasCurrentNodeMadeChoice() const {
				return pending_choice_ >= 0;
			}

			void ConstructNode() {
				assert(current_node_);
				assert(pending_choice_ >= 0);

				auto result = current_node_->GetOrCreateChild(pending_choice_, [&](ChildNodeMap & children) {
					return children.CreateNewNode(pending_choice_, std::make_unique<TreeNode>());
				});
				bool new_node_created = result.first;
				auto child = result.second;

				// Since a redirect node should only appear at the end of a main-action-sequence,
				// it should not be followed.
				assert(!child->IsRedirectNode());
				assert(child->GetNode());

				AddPathNode(current_node_, pending_choice_, &child->GetEdgeAddon(), child->GetNode());
				if (new_node_created) new_node_created_ = true;
			}

			void ConstructRedirectNode(detail::BoardNodeMap * redirect_node_map, engine::view::Board const& board, engine::Result result) {
				assert(current_node_);
				assert(pending_choice_ >= 0);

				auto child_result = current_node_->GetOrCreateChild(pending_choice_, [&](ChildNodeMap & children) {
					return children.CreateRedirectNode(pending_choice_);
				});
				bool new_node_created = child_result.first;
				auto child = child_result.second;

				if (new_node_created) new_node_created_ = true;

				if (result != engine::kResultNotDetermined) {
					// Don't need to construct a node for leaf nodes.
					// We only need the edge to record win-rate, which is already got before.
					TreeNode * next_node = nullptr;
					AddPathNode(current_node_, pending_choice_, &child->GetEdgeAddon(), next_node);
				}
				else {
					TreeNode * next_node = redirect_node_map->GetOrCreateNode(board, &new_node_created_);
					assert(next_node);
					assert(next_node->GetAddon().consistency_checker.CheckActionType(engine::ActionType::kMainAction));
					AddPathNode(current_node_, pending_choice_, &child->GetEdgeAddon(), next_node);
				}
			}

			void JumpToNode(engine::view::Board const& board) {
				assert(current_node_);
				assert(pending_choice_ < 0);
				TreeNode * next_node = current_node_->GetAddon().board_node_map.GetOrCreateNode(board);
				AddPathNode(current_node_, -1, nullptr, next_node);
			}

			void Update(double credit) {
				TreeUpdater updater;
				updater.Update(path_, credit);
			}

			auto const& GetPath() const { return path_; }

			bool HasNewNodeCreated() const { return new_node_created_; }

		private:
			void AddPathNode(TreeNode * node, int choice, EdgeAddon * edge_addon, TreeNode * next_node) {
				if (edge_addon) {
					edge_addon->AddChosenTimes(1);

					if constexpr (StaticConfigs::kVirtualLoss != 0) {
						static_assert(StaticConfigs::kVirtualLoss > 0);
						edge_addon->AddTotal(StaticConfigs::kVirtualLoss);
					}
				}

				if (next_node) {
					next_node->GetAddon().leading_nodes.AddLeadingNodes(node, edge_addon);
				}

				path_.emplace_back(node, choice, edge_addon);

				current_node_ = next_node;
				pending_choice_ = -1;
			}

		private:
			std::vector<TraversedNodeInfo> path_;
			bool new_node_created_;
			TreeNode * current_node_;
			int pending_choice_;
		};
	}
}
