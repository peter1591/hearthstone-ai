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

				auto result = current_node_->children_.GetOrCreateNewNode(
					pending_choice_, std::make_unique<TreeNode>());
				bool new_node_created = std::get<0>(result);
				auto edge_addon = std::get<1>(result);
				auto node = std::get<2>(result);

				assert(node);

				AddPathNode(current_node_, pending_choice_,edge_addon, node);
				if (new_node_created) new_node_created_ = true;
			}

			void ConstructRedirectNode(detail::BoardNodeMap * redirect_node_map, engine::view::Board const& board, engine::Result result) {
				assert(current_node_);
				assert(pending_choice_ >= 0);

				auto child_result = current_node_->children_.GetOrCreatRedirectNode(pending_choice_);
				bool new_node_created = std::get<0>(child_result);
				auto edge_addon = std::get<1>(child_result);
				auto node = std::get<2>(child_result);
				(void)node;
				assert(node == nullptr); // should be a redirect node

				if (new_node_created) new_node_created_ = true;

				if (result != engine::kResultNotDetermined) {
					// Don't need to construct a node for leaf nodes.
					// We only need the edge to record win-rate, which is already got before.
					TreeNode * next_node = nullptr;
					AddPathNode(current_node_, pending_choice_, edge_addon, next_node);
				}
				else {
					TreeNode * next_node = redirect_node_map->GetOrCreateNode(board, &new_node_created_);
					assert(next_node);
					assert(next_node->addon_.consistency_checker.CheckActionType(engine::ActionType::kMainAction));
					AddPathNode(current_node_, pending_choice_, edge_addon, next_node);
				}
			}

			void JumpToNode(engine::view::Board const& board) {
				assert(current_node_);
				assert(pending_choice_ < 0);
				TreeNode * next_node = current_node_->addon_.board_node_map.GetOrCreateNode(board);
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
					next_node->addon_.leading_nodes.AddLeadingNodes(node, edge_addon);
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
