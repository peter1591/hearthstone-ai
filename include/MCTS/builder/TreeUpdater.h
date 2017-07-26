#pragma once

#include "mcts/selection/TraversedNodeInfo.h"

namespace mcts
{
	namespace builder
	{
		class TreeUpdater
		{
		public:
			TreeUpdater() : last_node_(nullptr), nodes_()
#ifndef NDEBUG
				,should_visits_()
#endif
			{}

			TreeUpdater(TreeUpdater const&) = delete;
			TreeUpdater & operator=(TreeUpdater const&) = delete;

			void Update(bool credit)
			{
				UpdateChosenTimes();
				TreeLikeUpdateWinRate(credit);
				//LinearlyUpdateWinRate(credit);
			}

			void PushBackNodes(std::vector<selection::TraversedNodeInfo> & nodes, selection::TreeNode * last_node)
			{
				assert([&]() {
					for (size_t i = 0; i < nodes.size(); ++i) {
						auto const& item = nodes[i];
						
						if (!item.GetNode()) return false;

						// every node should have an edge to the next node,
						if (!item.GetEdgeAddon()) return false;

						if (i > 0) {
							if (nodes[i - 1].GetNode()->GetChildNode(nodes[i - 1].GetChoice())
								!= nodes[i].GetNode()) return false;
						}
					}
					return true;
				}());

				last_node_ = last_node;
				if (HasLastNode()) {
					if (nodes.front().GetNode() != last_node_) {
						nodes_.emplace_back(last_node_);
					}
				}

				std::move(nodes.begin(), nodes.end(), std::back_inserter(nodes_));
				nodes.clear();
			}

			void Clear()
			{
				last_node_ = nullptr;
				nodes_.clear();
			}

		private:
			// Note: should never touch the last_node, since it might be a sink node (win or lose)
			// sink node is at address 0x1 or 0x2
			bool HasLastNode() const {
				if (!last_node_) return false;
				if (last_node_->IsWinNode()) return false;
				return true;
			}

			void UpdateChosenTimes() {
				for (size_t i = 0; i < nodes_.size(); ++i) {
					auto const& item = nodes_[i];

					if (item.GetChoice() >= 0) {
						selection::TreeNodeAddon * next_node_addon = nullptr;
						if ((i + 1) < nodes_.size()) {
							next_node_addon = &nodes_[i + 1].GetNode()->GetAddon();
						}
						else {
							if (HasLastNode()) {
								next_node_addon = &last_node_->GetAddon();
							}
						}

						if (next_node_addon) {
							next_node_addon->leading_nodes.AddLeadingNodes(
								item.GetNode(), item.GetChoice());
						}
					}

					if (item.GetEdgeAddon()) {
						auto & edge_addon = *item.GetEdgeAddon();
						edge_addon.chosen_times++;
					}
				}
			}

			void LinearlyUpdateWinRate(bool credit) {
				for (auto const& item : nodes_) {
					auto * edge_addon = item.GetEdgeAddon();
					if (!edge_addon) continue;

					if (credit) ++edge_addon->credit;
					++edge_addon->total;
				}
			}

			void TreeLikeUpdateWinRate(bool credit) {
				if (nodes_.empty()) return;

				assert([&](){
					should_visits_.clear();
					for (auto const& item : nodes_) {
						if (item.GetEdgeAddon()) {
							should_visits_.insert(item.GetEdgeAddon());
						}
					}
					return true;
				}());

				auto it = nodes_.rbegin();
				while (it != nodes_.rend()) {
					TreeLikeUpdateWinRate(
						it->GetNode(),
						it->GetChoice(),
						credit);

					// skip to next redirect node
					// all intermediate nodes are already updated
					++it;
					while (it != nodes_.rend()) {
						if (it->GetChoice() < 0) break;
						++it;
					}

					assert(it->GetEdgeAddon() == nullptr);
					++it;
				}

				assert(should_visits_.empty());
			}

			void TreeLikeUpdateWinRate(selection::TreeNode * node, int choice, bool credit)
			{
				assert(node);

				auto * edge_addon = node->GetEdgeAddon(choice);
				assert(edge_addon);
				assert([&]() {
					should_visits_.erase(edge_addon);
					return true;
				}());
				if (credit) ++edge_addon->credit;
				++edge_addon->total;

				node->GetAddon().leading_nodes.ForEachLeadingNode(
					[&](selection::TreeNode * leading_node, int leading_choice)
				{
					TreeLikeUpdateWinRate(leading_node, leading_choice, credit);
					return true;
				});
			}

		private:
			selection::TreeNode * last_node_;
			std::vector<selection::TraversedNodeInfo> nodes_;

#ifndef NDEBUG
			std::unordered_set<selection::EdgeAddon*> should_visits_;
#endif
		};
	}
}