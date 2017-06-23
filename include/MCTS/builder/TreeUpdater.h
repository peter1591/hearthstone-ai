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
					item.node->ReportResult(credit);
				}
			}

			void PushBackNodes(std::vector<selection::TraversedNodeInfo> const& nodes)
			{
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