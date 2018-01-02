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

				auto result = current_node_->FollowChoice(pending_choice_);
				AddPathNode(current_node_, pending_choice_, &result.edge_addon, result.node);
				new_node_created_ = result.just_expanded;
			}

			void ConstructRedirectNode(detail::BoardNodeMap * turn_node_map, engine::view::Board const& board, engine::Result result) {
				assert(current_node_);
				assert(pending_choice_ >= 0);
				auto & edge_addon = current_node_->MarkChoiceRedirect(pending_choice_);

				if (result != engine::kResultNotDetermined) {
					// Don't need to construct a node for leave nodes.
					// We only need the edge to record win-rate, which is already got before.
					TreeNode * next_node = nullptr;
					AddPathNode(current_node_, pending_choice_, &edge_addon, next_node);
				}
				else {
					TreeNode * next_node = turn_node_map->GetOrCreateNode(board, &new_node_created_);
					assert(next_node);
					assert([&](selection::TreeNode* node) {
						if (!node->GetActionType().IsValid()) return true;
						return node->GetActionType().GetType() == engine::ActionType::kMainAction;
					}(next_node));
					AddPathNode(current_node_, pending_choice_, &edge_addon, next_node);
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
				updater.Update(path_, current_node_, credit);
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

				path_.emplace_back(node, choice, edge_addon);

				assert(next_node);
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
