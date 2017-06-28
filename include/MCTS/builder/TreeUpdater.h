#pragma once

#include "mcts/selection/TraversedNodeInfo.h"

namespace mcts
{
	namespace builder
	{
		class TreeUpdater
		{
		public:
			void Update(bool credit)
			{
				for (auto const& item : nodes_) {
					auto & statistic = item.node->GetAddon().statistic;
					if (credit) ++statistic.credit;
					++statistic.total;

					if (item.edge_addon) {
						++item.edge_addon->chosen_times;
					}
				}
			}

			void PushBackNodes(std::vector<selection::TraversedNodeInfo> const& nodes)
			{
				assert([&]() {
					bool first = true;
					for (auto const& item : nodes) {
						if (!item.node) return false;

						// edge_addon should be there unless it's the first one
						if (item.leading_choice >= 0) { // not a randomly-choose action
							if (!item.edge_addon) return false;
						}
						first = false;
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