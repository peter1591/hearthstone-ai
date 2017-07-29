#pragma once

#include <mutex>
#include "MCTS/detail/BoardNodeMap.h"
#include "MCTS/board/BoardView.h"
#include "MCTS/board/ActionChoices.h"
#include "Utils/HashCombine.h"

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

			bool Check(
				board::Board const& board,
				ActionType action_type,
				board::ActionChoices const& choices)
			{
				std::lock_guard<std::mutex> lock(mutex_);

				assert(action_type.IsValid());
				if (!CheckActionType(action_type)) return false;
				if (!LockedCheckBoard(board.CreateView())) return false;
				return true;
			}

			bool CheckBoard(board::BoardView const& new_view) {
				std::lock_guard<std::mutex> lock(mutex_);
				return LockedCheckBoard(new_view);
			}

		private:
			bool LockedCheckBoard(board::BoardView const& new_view) {
				if (!board_view_) {
					board_view_.reset(new board::BoardView(new_view));
					return true;
				}
				return *board_view_ == new_view;
			}

			bool CheckActionType(ActionType action_type) {
				if (!action_type_.IsValid()) {
					action_type_ = action_type;
					return true;
				}
				return action_type_ == action_type;
			}

		private:
			std::mutex mutex_;
			std::unique_ptr<board::BoardView> board_view_;
			ActionType action_type_;
		};

		class TreeNodeLeadingNodes
		{
		public:
			TreeNodeLeadingNodes() : mutex_(), items_() {}

			void AddLeadingNodes(TreeNode * node, int choice) {
				std::lock_guard<std::shared_mutex> lock(mutex_);
				assert(node);
				assert(choice >= 0);
				items_.insert(TreeNodeLeadingNodesItem{ node, choice });
			}

			template <class Functor>
			void ForEachLeadingNode(Functor&& op) {
				std::shared_lock<std::shared_mutex> lock(mutex_);
				for (auto const& item : items_) {
					if (!op(item.node, item.choice)) break;
				}
			}

		private:
			mutable std::shared_mutex mutex_;

			// both node and choice are in the key field,
			// since different choices might need to an identical node
			// these might happened when trying to heal two different targets,
			// but both targets are already fully-healed
			std::unordered_set<TreeNodeLeadingNodesItem> items_;
		};

		// Add abilities to tree node to use in SO-MCTS
		// Note: this will lived in *every* tree node, so careful about memory footprints
		// Thread safety: No.
		struct TreeNodeAddon
		{
			TreeNodeAddon() :
				action_analyzer(),
				consistency_checker(),
				board_node_map(),
				leading_nodes()
			{}

			board::BoardActionAnalyzer action_analyzer;
			TreeNodeConsistencyCheckAddons consistency_checker; // TODO: debug only
			detail::BoardNodeMap board_node_map;
			TreeNodeLeadingNodes leading_nodes;
		};
	}
}