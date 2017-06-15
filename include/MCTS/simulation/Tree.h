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
		//    sometimes a (sub-)action might still yields an invalid game state
		// In this case, we want to record the path of (sub-)actions yielding an invalid state
		//    and prevent this path for a following re-trial
		// Optimization: The case we yield an invalid game state is rarely happened
		// Idea:
		//   Delay the allocation of tree node when an invalid state is found
		class Tree
		{
		private:
			struct PendingAction {
				int choices;
				int choice;
			};
			struct TraversedInfo {
				TreeNode* node;
				int choice;
			};

		public:
			Tree() : node_(&root_), last_choice_(-1) {}

			void Clear() {
				root_.Reset();
				Restart();
			}

			void Restart() {
				node_ = &root_;
				last_choice_ = -1;
				pending_actions_.clear();
			}

			void FillChoices(int choices) {
				assert(node_);

				if (last_choice_ < 0) {
					// special case for the very first call
					assert(node_ == &root_);
					node_->Initialize(choices);
					return;
				}

				if (pending_actions_.empty()) {
					TreeNode* next_node = node_->GetChoice(last_choice_);
					if (!next_node) {
						// No tree node exists -> change to pending-action mode
						first_choice_before_pending_actions_ = last_choice_;
						pending_actions_.push_back({ choices, -1 });
					}
					else {
						AdvanceNode(last_choice_, next_node);
					}
				}
				else {
					assert(pending_actions_.back().choices > 0);
					pending_actions_.back().choice = last_choice_;
					pending_actions_.push_back({ choices, -1 });
				}
			}

			size_t GetWhiteListCount() const {
				assert(node_);
				if (pending_actions_.empty()) {
					assert(node_->HasAnyChoice());
					return node_->GetChoiceCount();
				}
				else {
					return pending_actions_.back().choices;
				}
			}
			size_t GetWhiteListItem(size_t idx) const {
				if (pending_actions_.empty()) {
					return node_->GetWhiteListChoice(idx);
				}
				else {
					return idx;
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
					for (int i = 0; i < pending_actions_.back().choices; ++i) {
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

					int next_edge = first_choice_before_pending_actions_;

					for (auto const& pending_action : pending_actions_) {
						TreeNode* next_node = node_->GetChoice(next_edge);
						if (!next_node) {
							next_node = node_->AllocateChoiceNode(next_edge);
							next_node->Initialize(pending_action.choices);
						}
						
						AdvanceNode(next_edge, next_node);
						next_edge = pending_action.choice;
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
			TreeNode root_;

			TreeNode* node_;
			int last_choice_;

			std::vector<TraversedInfo> traversed_nodes_;

			// The case for an invalid game state is rare
			// So we want to delay the creation of the TreeNode as late as possible
			// These fields remembers the pending, which is used to create the necessary
			//    TreeNodes when necessary.
			int first_choice_before_pending_actions_;
			std::vector<PendingAction> pending_actions_;
		};
	}
}