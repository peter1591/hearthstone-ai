#pragma once

#include <assert.h>
#include <memory>
#include <algorithm>

#include "MCTS/simulation/TreeNode.h"

namespace mcts
{
	namespace simulation
	{
		// Some (sub-)action results in an invalid game state
		// Ideally, we should prevent this situation as much as possible
		// However, for performance consideration and code simplicity,
		//    we don't want to do so many pre-checking, so
		//    sometimes a (sub-)action might still yields an invalid game state
		// In this case, we want to record the path of (sub-)actions yielding an invalid state
		//    and prevent this path for a following re-trial
		// Optimization: The case we yield an invalid game state is rarely happened
		//   so we delay the allocation of a tree node as late as possible
		class TreeTraverseProgress
		{
		private:
			struct PendingAction {
				int prev_choice;
				int current_choices;
			};
			struct TraversedInfo {
				TreeNode* node;
				int choice;
			};

		public:
			TreeTraverseProgress(TreeNode* node) :
				node_(node), last_choice_(-1), traversed_nodes_(),
				pending_actions_()
			{}

			TreeTraverseProgress(TreeTraverseProgress const&) = delete;
			TreeTraverseProgress & operator=(TreeTraverseProgress const&) = delete;

			void Reset(TreeNode* node) {
				node_ = node;
				last_choice_ = -1;
				traversed_nodes_.clear();
				pending_actions_.clear();
			}

			// @return nullptr, if current node is not allocated yet (delay-allocate)
			//         pointer to the addon, if current node exists
			TreeNodeAddon * GetCurrentNodeAddon() {
				if (pending_actions_.empty()) {
					assert(node_);
					return &node_->GetAddon();
				}
				return nullptr;
			}

			void FillChoices(int choices) {
				assert(node_);

				if (last_choice_ < 0) {
					// special case for the very first call
					node_->Initialize(choices);
					return;
				}

				if (pending_actions_.empty()) {
					TreeNode* next_node = node_->GetChoice(last_choice_);
					if (!next_node) {
						// No tree node exists -> change to pending-action mode
						pending_actions_.push_back({ last_choice_, choices });
					}
					else {
						AdvanceNode(last_choice_, next_node);
					}
				}
				else {
					assert(pending_actions_.back().current_choices > 0);
					pending_actions_.push_back({ last_choice_, choices });
				}
			}

			size_t GetWhiteListCount() const {
				assert(node_);
				if (pending_actions_.empty()) {
					assert(node_->HasAnyChoice());
					return node_->GetChoiceCount();
				}
				else {
					return pending_actions_.back().current_choices;
				}
			}
			int GetWhiteListItem(size_t idx) const {
				if (pending_actions_.empty()) {
					return (int)node_->GetWhiteListChoice(idx);
				}
				else {
					return (int)idx;
				}
			}
			template <typename Functor>
			void ForEachWhiteListItem(Functor&& functor) const {
				assert(node_);
				if (pending_actions_.empty()) {
					node_->ForEachWhiteListChoice([&](int choice, TreeNode* node) {
						return functor(choice);
					});
				}
				else {
					for (int i = 0; i < pending_actions_.back().current_choices; ++i) {
						if (!functor(i)) return;
					}
				}
			}

			void ApplyChoice(int choice) {
				assert(choice >= 0);
				last_choice_ = choice;
			}

			void ReportInvalidChoice() {
				if (!pending_actions_.empty()) {
					assert(node_);

					for (auto const& pending_action : pending_actions_) {
						int next_edge = pending_action.prev_choice;
						TreeNode* next_node = node_->GetChoice(next_edge);

						assert(!next_node); // if node is allocated, it should be stepped when traversal
						next_node = node_->AllocateChoiceNode(next_edge);
						next_node->Initialize(pending_action.current_choices);

						AdvanceNode(next_edge, next_node);
					}
				}

				assert(node_);
				node_->RemoveChoice(last_choice_);

				CascadeRemoveParentNodesWithNoChild();
			}

		private:
			void AdvanceNode(int choice, TreeNode* next_node)
			{
				assert(next_node);
				assert(node_->GetChoice(choice) == next_node);

				traversed_nodes_.push_back({ node_, choice });
				node_ = next_node;
			}

			void CascadeRemoveParentNodesWithNoChild() {
				if (node_->HasAnyChoice()) return;

				for (auto it = traversed_nodes_.rbegin(); it != traversed_nodes_.rend(); ++it) {
					TreeNode* parent = it->node;
					int choice = it->choice;

					parent->RemoveChoice(choice);
					if (parent->HasAnyChoice()) break;
				}
			}

		private:
			TreeNode* node_;
			int last_choice_;

			std::vector<TraversedInfo> traversed_nodes_;

			// The case for an invalid game state is rare
			// So we want to delay the creation of the TreeNode as late as possible
			// These fields remembers the pending, which is used to create the necessary
			//    TreeNodes when necessary.
			std::vector<PendingAction> pending_actions_;
		};
	}
}