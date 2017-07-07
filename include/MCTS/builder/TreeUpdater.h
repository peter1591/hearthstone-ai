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
					auto & statistic = item.GetNode()->GetAddon().statistic;
					if (credit) ++statistic.credit;
					++statistic.total;

					assert(item.GetEdgeAddon());
					item.GetEdgeAddon()->chosen_times++;
				}
			}

			void PushBackNodes(std::vector<selection::TraversedNodeInfo> const& nodes)
			{
				assert([&]() {
					for (auto const& item : nodes) {
						if (!item.GetNode()) return false;
						if (!item.GetEdgeAddon()) return false;
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