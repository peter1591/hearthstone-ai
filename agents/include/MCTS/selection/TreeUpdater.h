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
			TreeUpdater() : last_node_(nullptr), nodes_(), bfs_()
#ifndef NDEBUG
				, should_visits_(), pushed_terminate_node_(false)
#endif
			{}

			TreeUpdater(TreeUpdater const&) = delete;
			TreeUpdater & operator=(TreeUpdater const&) = delete;

			void Update(double credit)
			{
				UpdateChosenTimes();
				TreeLikeUpdateWinRate(credit);
				//LinearlyUpdateWinRate(credit);
			}

			void PushTerminateNode(std::vector<selection::TraversedNodeInfo> & nodes, engine::Result result)
			{
				(void)result; // don't need result. we just need push the last node as nullptr.
				PushBackNodes(nodes, nullptr);

				assert([&]() {
					pushed_terminate_node_ = true;
					return true;
				}());
			}

			void PushBackNodes(std::vector<selection::TraversedNodeInfo> & nodes, selection::TreeNode * last_node)
			{
				assert([&]() {
					for (size_t i = 0; i < nodes.size(); ++i) {
						auto const& item = nodes[i];
						
						if (!item.node_) return false;

						// every node should have an edge to the next node,
						if (!item.edge_addon_) return false;

						if (i > 0) {
							if (nodes[i - 1].node_->GetChildNode(nodes[i - 1].choice_)
								!= nodes[i].node_) return false;
						}
					}
					return true;
				}());

				assert(!pushed_terminate_node_);

				if (last_node_) {
					if (nodes.front().node_ != last_node_) {
						nodes_.emplace_back(last_node_, -1, nullptr);
					}
				}
				last_node_ = last_node;

				for (auto & new_node : nodes) {
					auto * edge_addon = new_node.edge_addon_;
					if (edge_addon) {
						edge_addon->AddTotal(StaticConfigs::kVirtualLoss);
					}
					nodes_.emplace_back(std::move(new_node));
				}
				nodes.clear();
			}

			void Clear()
			{
				last_node_ = nullptr;
				nodes_.clear();
				assert([&]() {
					pushed_terminate_node_ = false;
					return true;
				}());
			}

		private:
			void UpdateChosenTimes() {
				for (size_t i = 0; i < nodes_.size(); ++i) {
					auto const& item = nodes_[i];

					if (item.choice_ >= 0) {
						selection::TreeNodeAddon * next_node_addon = nullptr;
						if ((i + 1) < nodes_.size()) {
							next_node_addon = &nodes_[i + 1].node_->GetAddon();
						}
						else {
							if (last_node_) {
								next_node_addon = &last_node_->GetAddon();
							}
						}

						if (next_node_addon) {
							next_node_addon->leading_nodes.AddLeadingNodes(
								item.node_, item.choice_);
						}
					}

					if (item.edge_addon_) {
						auto & edge_addon = *item.edge_addon_;
						edge_addon.AddChosenTimes(1);

						edge_addon.AddTotal(-StaticConfigs::kVirtualLoss); // remove virtual loss
						assert(edge_addon.GetTotal() >= 0);
					}
				}
			}

			void LinearlyUpdateWinRate(double credit) {
				for (auto const& item : nodes_) {
					auto * edge_addon = item.edge_addon_;
					if (!edge_addon) continue;

					edge_addon->AddTotal(100);
					edge_addon->AddCredit((int)(credit * 100.0));
				}
			}

			void TreeLikeUpdateWinRate(double credit) {
				if (nodes_.empty()) return;

				assert([&](){
					should_visits_.clear();
					for (auto const& item : nodes_) {
						if (item.edge_addon_) {
							should_visits_.insert(item.edge_addon_);
						}
					}
					return true;
				}());

				auto it = nodes_.rbegin();
				while (it != nodes_.rend()) {
					TreeLikeUpdateWinRate(
						it->node_,
						it->choice_,
						credit);

					// skip to next redirect node
					// all intermediate nodes are already updated
					++it;
					while (it != nodes_.rend()) {
						if (it->choice_ < 0) break;
						++it;
					}
					if (it == nodes_.rend()) break;

					assert(it->edge_addon_ == nullptr);
					++it;
				}

				assert(should_visits_.empty());
			}

			void TreeLikeUpdateWinRate(selection::TreeNode * start_node, int start_choice, double credit)
			{
				assert(start_node);
				
				assert(bfs_.empty());
				bfs_.push({ start_node, start_choice });

				while (!bfs_.empty()) {
					auto node = bfs_.front().node;
					int choice = bfs_.front().choice;
					auto * edge_addon = node->GetEdgeAddon(choice);
					bfs_.pop();

					assert(edge_addon);
					assert([&]() {
						should_visits_.erase(edge_addon);
						return true;
					}());
					edge_addon->AddTotal(100);
					edge_addon->AddCredit((int)(credit*100.0));

					// use BFS to reduce the lock time
					node->GetAddon().leading_nodes.ForEachLeadingNode(
						[&](selection::TreeNode * leading_node, int leading_choice)
					{
						bfs_.push({ leading_node, leading_choice });
						return true;
					});
				}
			}

		private:
			selection::TreeNode * last_node_;
			std::vector<selection::TraversedNodeInfo> nodes_;

			struct Item {
				selection::TreeNode * node;
				int choice;
			};
			std::queue<Item> bfs_;

#ifndef NDEBUG
			std::unordered_set<selection::EdgeAddon*> should_visits_;
			bool pushed_terminate_node_;
#endif
		};
	}
}
