#pragma once

#include <mutex>
#include "engine/ActionType.h"
#include "MCTS/detail/BoardNodeMap.h"
#include "MCTS/selection/EdgeAddon.h"
#include "engine/view/ReducedBoardView.h"
#include "Utils/HashCombine.h"
#include "Utils/SpinLocks.h"

namespace mcts
{
	namespace selection
	{
		struct TreeNodeLeadingNodesItem {
			// We need to store both *leading node* and *edge* to support redirect nodes.
			// Since *edge* might be coming from a board node map

			TreeNode * node;
			EdgeAddon * edge_addon;

			bool operator==(TreeNodeLeadingNodesItem const& v) const {
				if (node != v.node) return false;
				if (edge_addon != v.edge_addon) return false;
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
			Utils::HashCombine::hash_combine(result, v.edge_addon);
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
			TreeNodeConsistencyCheckAddons() : mutex_(), board_view_(), action_type_(), action_choices_() {}

			bool SetAndCheck(
				engine::ActionType action_type,
				engine::ActionChoices const& choices)
			{
				std::lock_guard<Utils::SpinLock> lock(mutex_);

				assert(action_type.IsValid());
				if (!CheckActionTypeAndChoices(action_type, choices)) return false;
				return true;
			}

			bool SetAndCheckBoard(engine::view::ReducedBoardView const& new_view) {
				std::lock_guard<Utils::SpinLock> lock(mutex_);
				return LockedSetAndCheckBoard(new_view);
			}

			bool CheckBoard(engine::view::ReducedBoardView const& new_view) const {
				std::lock_guard<Utils::SpinLock> lock(mutex_);
				if (!board_view_) return true;
				return *board_view_ == new_view;
			}

			bool CheckActionType(engine::ActionType action_type) const {
				std::lock_guard<Utils::SpinLock> lock(mutex_);
				return LockedCheckActionType(action_type);
			}

			auto GetActionType() const {
				std::lock_guard<Utils::SpinLock> lock(mutex_);
				return action_type_;
			}

			auto GetBoard() const {
				std::lock_guard<Utils::SpinLock> lock(mutex_);
				return board_view_.get();
			}

		private:
			bool LockedSetAndCheckBoard(engine::view::ReducedBoardView const& new_view) {
				if (!board_view_) {
					board_view_.reset(new engine::view::ReducedBoardView(new_view));
					return true;
				}
				return *board_view_ == new_view;
			}

			bool LockedCheckActionType(engine::ActionType action_type) const {
				if (!action_type_.IsValid()) return true;
				return action_type_ == action_type;
			}

			bool CheckActionTypeAndChoices(engine::ActionType action_type, engine::ActionChoices const& choices) {
				if (!action_type_.IsValid()) {
					action_type_ = action_type;
					action_choices_ = choices;
					return true;
				}

				if (action_type_ != action_type) return false;

				if (action_choices_.GetIndex() != choices.GetIndex()) return false;
				if (!action_choices_.Compare(choices, [](auto&& lhs, auto&& rhs) {
					using Type1 = std::decay_t<decltype(lhs)>;
					using Type2 = std::decay_t<decltype(rhs)>;

					if constexpr (std::is_same_v<Type1, engine::ActionChoices::ChooseFromCardIds>) {
						// allow different set of card ids
						return true;
					}
					else if constexpr (std::is_same_v<Type1, engine::ActionChoices::ChooseFromZeroToExclusiveMax>) {
						return lhs.Size() == rhs.Size();
					}
					else if constexpr (std::is_same_v<Type1, engine::ActionChoices::InvalidChoice>) {
						return false; // should be valid choices
					}
					else {
						return false; // type mismatch
					}
				})) return false;

				return true;
			}

		private:
			mutable Utils::SpinLock mutex_;
			std::unique_ptr<engine::view::ReducedBoardView> board_view_;
			engine::ActionType action_type_;
			engine::ActionChoices action_choices_;
		};

		class TreeNodeLeadingNodes
		{
		public:
			TreeNodeLeadingNodes() : mutex_(), items_() {}

			void AddLeadingNodes(TreeNode * node, EdgeAddon * edge_addon) {
				std::lock_guard<Utils::SharedSpinLock> lock(mutex_);
				assert(node);

				for (auto const& item : items_) {
					if (item.node == node && item.edge_addon == edge_addon) {
						// duplicated, ignore
						return;
					}
				}

				items_.push_back(TreeNodeLeadingNodesItem{ node, edge_addon });
			}

			template <class Functor>
			void ForEachLeadingNode(Functor&& op) {
				std::shared_lock<Utils::SharedSpinLock> lock(mutex_);
				for (auto const& item : items_) {
					if (!op(item.node, item.edge_addon)) break;
				}
			}

		private:
			mutable Utils::SharedSpinLock mutex_;
			std::vector<TreeNodeLeadingNodesItem> items_;
		};

		// Add abilities to tree node to use in SO-MCTS
		// Note: this will live in *every* tree node, so careful about memory footprints
		// Thread safety: No.
		// TODO: should be thread safe
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