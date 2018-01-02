#pragma once

#include <queue>
#include "MCTS/selection/TraversedNodeInfo.h"
#include "MCTS/selection/TreeUpdater.h"
#include "MCTS/Config.h"

namespace mcts
{
	namespace selection
	{
		class TreeUpdater
		{
		public:
			TreeUpdater() : bfs_()
#ifndef NDEBUG
				, should_visits_()
#endif
			{}

			TreeUpdater(TreeUpdater const&) = delete;
			TreeUpdater & operator=(TreeUpdater const&) = delete;

			void Update(std::vector<selection::TraversedNodeInfo> const& nodes, float credit)
			{
				for (size_t i = 0; i < nodes.size(); ++i) {
					auto const& item = nodes[i];

					if (item.edge_addon_) {
						if constexpr (StaticConfigs::kVirtualLoss != 0) {
							static_assert(StaticConfigs::kVirtualLoss > 0);
							item.edge_addon_->AddCredit(1.0, StaticConfigs::kVirtualLoss); // remove virtual loss
							assert(item.edge_addon_->GetAverageCredit() >= 0.0);
							assert(item.edge_addon_->GetAverageCredit() <= 1.0);
						}
					}
				}

				TreeLikeUpdateWinRate(nodes, credit);
				//LinearlyUpdateWinRate(nodes, credit);
			}

		private:
			void LinearlyUpdateWinRate(std::vector<selection::TraversedNodeInfo> const& nodes, float credit) {
				for (auto const& item : nodes) {
					auto * edge_addon = item.edge_addon_;
					if (!edge_addon) continue;
					edge_addon->AddCredit(credit);
				}
			}

			void TreeLikeUpdateWinRate(std::vector<selection::TraversedNodeInfo> const& nodes, float credit) {
				if (nodes.empty()) return;

				assert([&](){
					should_visits_.clear();
					for (auto const& item : nodes) {
						if (item.edge_addon_) {
							should_visits_.insert(item.edge_addon_);
						}
					}
					return true;
				}());

				for (auto it = nodes.crbegin(); it != nodes.crend(); ++it) {
					if (!it->edge_addon_) continue;
					TreeLikeUpdateWinRate(it->node_, it->edge_addon_, credit);
					break;
				}

				assert(should_visits_.empty());
			}

			void TreeLikeUpdateWinRate(selection::TreeNode * start_node, EdgeAddon * start_edge, float credit)
			{
				assert(start_node);
				
				assert(bfs_.empty());
				bfs_.push({ start_node, start_edge });

				while (!bfs_.empty()) {
					auto node = bfs_.front().node;
					auto * edge_addon = bfs_.front().edge_addon;
					bfs_.pop();

					if (edge_addon) {
						assert([&]() {
							should_visits_.erase(edge_addon);
							return true;
						}());
						edge_addon->AddCredit(credit);
					}

					// use BFS to reduce the lock time
					node->addon_.leading_nodes.ForEachLeadingNode(
						[&](selection::TreeNode * leading_node, EdgeAddon *leading_edge)
					{
						// TODO: search for identitical nodes. if found, just update it multiple times. don't need to traverse multiple times
						bfs_.push({ leading_node, leading_edge });
						return true;
					});
				}
			}

		private:
			struct Item {
				TreeNode * node;
				EdgeAddon * edge_addon;
			};
			std::queue<Item> bfs_;

#ifndef NDEBUG
			std::unordered_set<EdgeAddon*> should_visits_;
#endif
		};
	}
}
