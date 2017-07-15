#pragma once

#include "mcts/selection/TraversedNodeInfo.h"

namespace mcts
{
	namespace builder
	{
		class TreeUpdater
		{
		public:
			TreeUpdater() : nodes_() {}

			void Update(bool credit)
			{
				for (auto const& item : nodes_) {
					if (item.GetEdgeAddon()) {
						auto & edge_addon = *item.GetEdgeAddon();
						edge_addon.chosen_times++;

						// to calculate win-rate
						if (credit) ++edge_addon.credit;
						++edge_addon.total;
					}
				}
			}

			void PushBackNodes(std::vector<selection::TraversedNodeInfo> const& nodes)
			{
				assert([&]() {
					for (size_t i = 0; i < nodes.size(); ++i) {
						auto const& item = nodes[i];
						
						if (!item.GetNode()) return false;

						// every node should have an edge to the next node,
						// except for the last node
						if (i < nodes.size() - 1) {
							if (!item.GetEdgeAddon()) return false;
						}

						if (i > 0) {
							if (nodes[i - 1].GetNode()->GetChildNode(nodes[i - 1].GetChoice())
								!= nodes[i].GetNode()) return false;
						}
					}
					return true;
				}());
				nodes_.insert(nodes_.end(), nodes.begin(), nodes.end());
			}

			void Clear()
			{
				nodes_.clear();
			}

		private:
			std::vector<selection::TraversedNodeInfo> nodes_;
		};
	}
}