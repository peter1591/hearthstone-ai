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
				AddPathNode(current_node_, pending_choice_, &result.edge_addon);
				new_node_created_ = result.just_expanded;
				
				assert(result.node);
				current_node_ = result.node;
				pending_choice_ = -1;
			}

			void ConstructRedirectNode(detail::BoardNodeMap * turn_node_map, engine::view::Board const& board, engine::Result result) {
				assert(current_node_);
				assert(pending_choice_ >= 0);
				auto & edge_addon = current_node_->MarkChoiceRedirect(pending_choice_);
				AddPathNode(current_node_, pending_choice_, &edge_addon);

				if (result != engine::kResultNotDetermined) {
					// Don't need to construct a node for leave nodes.
					// We only need the edge to record win-rate, which is already got before.
					current_node_ = nullptr;
					pending_choice_ = -1;
				}
				else {
					current_node_ = turn_node_map->GetOrCreateNode(board, &new_node_created_);
					pending_choice_ = -1;
					assert(current_node_);
					assert([&](selection::TreeNode* node) {
						if (!node->GetActionType().IsValid()) return true;
						return node->GetActionType().GetType() == engine::ActionType::kMainAction;
					}(current_node_));
				}
			}

			void JumpToNode(engine::view::Board const& board) {
				assert(current_node_);
				assert(pending_choice_ < 0);
				AddPathNode(current_node_, -1, nullptr);
				current_node_ = current_node_->GetAddon().board_node_map.GetOrCreateNode(board);
			}

			auto const& GetPath() const { return path_; }

			bool HasNewNodeCreated() const { return new_node_created_; }

		private:
			void AddPathNode(TreeNode * node, int choice, EdgeAddon * edge_addon) {
				path_.emplace_back(node, choice, edge_addon);

				if (edge_addon) {
					edge_addon->AddChosenTimes(1);

					if constexpr (StaticConfigs::kVirtualLoss != 0) {
						static_assert(StaticConfigs::kVirtualLoss > 0);
						edge_addon->AddTotal(StaticConfigs::kVirtualLoss);
					}
				}
			}

		private:
			std::vector<TraversedNodeInfo> path_;
			bool new_node_created_;
			TreeNode * current_node_;
			int pending_choice_;
		};
	}
}