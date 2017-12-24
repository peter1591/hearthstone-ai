#pragma once

#include <memory>
#include <unordered_map>
#include "engine/view/Board.h"
#include "Utils/SpinLocks.h"

namespace mcts
{
	namespace selection { class TreeNode; }

	namespace detail
	{
		class BoardNodeMap
		{
		private:
			using TreeNode = mcts::selection::TreeNode;
			using MapType = std::unordered_map<engine::view::ReducedBoardView, std::unique_ptr<TreeNode>>;

		public:
			BoardNodeMap() : mutex_(), map_() {}

			TreeNode* GetOrCreateNode(engine::view::Board const& board, bool * new_node_created = nullptr);

			template <typename Functor>
			void ForEach(Functor&& functor) const {
				std::shared_lock<Utils::SharedSpinLock> lock_(mutex_);

				if (!map_) return;
				for (auto const& kv : *map_) {
					if (!functor(kv.first, kv.second.get())) return;
				}
			}

		private:
			MapType & GetMap()
			{
				if (!map_) map_.reset(new MapType());
				return *map_.get();
			}

		private:
			mutable Utils::SharedSpinLock mutex_;
			std::unique_ptr<MapType> map_;
		};
	}
}