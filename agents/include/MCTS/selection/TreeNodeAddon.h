#pragma once

#include <mutex>
#include "engine/ActionType.h"
#include "MCTS/detail/BoardNodeMap.h"
#include "engine/view/BoardView.h"
#include "Utils/HashCombine.h"
#include "Utils/SpinLocks.h"

namespace mcts
{
	namespace selection
	{
		struct TreeNodeLeadingNodesItem {
			TreeNode * node;
			int choice;

			bool operator==(TreeNodeLeadingNodesItem const& v) const {
				if (node != v.node) return false;
				if (choice != v.choice) return false;
				return true;
			}

			bool operator!=(TreeNodeLeadingNodesItem const& v) const {
				return !(*this == v);
			}
		};
	}
}

namespace std {
	template <>
	struct hash<mcts::selection::TreeNodeLeadingNodesItem> {
		inline std::size_t operator()(mcts::selection::TreeNodeLeadingNodesItem const& v) const
		{
			std::size_t result = std::hash<mcts::selection::TreeNode*>()(v.node);
			Utils::HashCombine::hash_combine(result, v.choice);
			return result;
		}
	};
}

namespace mcts
{
	namespace selection
	{
		class TreeNodeConsistencyCheckAddons
		{
		public:
			TreeNodeConsistencyCheckAddons() : mutex_(), board_view_(), action_type_() {}

			bool SetAndCheck(
				engine::view::Board const& board,
				engine::ActionType action_type,
				engine::ActionChoices const& choices)
			{
				std::lock_guard<Utils::SpinLock> lock(mutex_);

				assert(action_type.IsValid());
				if (!CheckActionType(action_type)) return false;
				if (!LockedCheckBoard(board.CreateView())) return false;
				return true;
			}

			bool CheckBoard(engine::view::BoardView const& new_view) {
				std::lock_guard<Utils::SpinLock> lock(mutex_);
				return LockedCheckBoard(new_view);
			}

			auto GetBoard() const {
				std::lock_guard<Utils::SpinLock> lock(mutex_);
				return board_view_.get();
			}

		private:
			bool LockedCheckBoard(engine::view::BoardView const& new_view) {
				if (!board_view_) {
					board_view_.reset(new engine::view::BoardView(new_view));
					return true;
				}
				return *board_view_ == new_view;
			}

			bool CheckActionType(engine::ActionType action_type) {
				if (!action_type_.IsValid()) {
					action_type_ = action_type;
					return true;
				}
				return action_type_ == action_type;
			}

		private:
			mutable Utils::SpinLock mutex_;
			std::unique_ptr<engine::view::BoardView> board_view_;
			engine::ActionType action_type_;
		};

		class TreeNodeLeadingNodes
		{
		public:
			TreeNodeLeadingNodes() : mutex_(), items_() {}

			void AddLeadingNodes(TreeNode * node, int choice) {
				std::lock_guard<Utils::SharedSpinLock> lock(mutex_);
				assert(node);
				assert(choice >= 0);
				items_.insert(TreeNodeLeadingNodesItem{ node, choice });
			}

			template <class Functor>
			void ForEachLeadingNode(Functor&& op) {
				std::shared_lock<Utils::SharedSpinLock> lock(mutex_);
				for (auto const& item : items_) {
					if (!op(item.node, item.choice)) break;
				}
			}

		private:
			mutable Utils::SharedSpinLock mutex_;

			// both node and choice are in the key field,
			// since different choices might need to an identical node
			// these might happened when trying to heal two different targets,
			// but both targets are already fully-healed
			std::unordered_set<TreeNodeLeadingNodesItem> items_;
		};

		// Add abilities to tree node to use in SO-MCTS
		// Note: this will live in *every* tree node, so careful about memory footprints
		// Thread safety: No.
		struct TreeNodeAddon
		{
			TreeNodeAddon() :
				consistency_checker(),
				board_node_map(),
				leading_nodes()
			{}

			TreeNodeConsistencyCheckAddons consistency_checker; // TODO: debug only
			detail::BoardNodeMap board_node_map;
			TreeNodeLeadingNodes leading_nodes;
		};
	}
}