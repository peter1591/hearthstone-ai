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
			Tree() : root_(-1), node_(&root_), last_choice_(-1)
			{}

			void Clear() {
				root_.Clear();
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
					node_->SetChoices(choices);
					return;
				}

				if (pending_actions_.empty()) {
					auto it = node_->GetWhiteList().find(last_choice_);
					assert(it != node_->GetWhiteList().end()); // one should not choose a black-listed action
					if (!it->second) {
						// No tree node exists -> change to pending-action mode
						first_choice_before_pending_actions_ = last_choice_;
						pending_actions_.push_back({ choices, -1 });
					}
					else {
						AdvanceNode(last_choice_, it->second.get());
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
					assert(node_->GetWhiteList().size() > 0);
					return node_->GetWhiteList().size();
				}
				else {
					return pending_actions_.back().choices;
				}
			}
			template <typename Functor>
			void ForEachWhiteListItem(Functor&& functor) {
				assert(node_);
				if (pending_actions_.empty()) {
					for (auto it = node_->GetWhiteList().begin();
						it != node_->GetWhiteList().end();
						++it)
					{
						if (!functor(it->first)) return;
					}
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
						auto & node_container = node_->GetWhiteList()[next_edge];
						if (!node_container.get()) {
							TreeNode* new_node = new TreeNode(next_edge);
							new_node->SetChoices(pending_action.choices);
							node_container.reset(new_node);
						}

						AdvanceNode(next_edge, node_container.get());
						next_edge = pending_action.choice;
					}
				}

				assert(node_);
				auto it = node_->GetWhiteList().find(last_choice_);
				assert(it != node_->GetWhiteList().end()); // one should not choose a black-listed action
				node_->GetWhiteList().erase(it);

				CascadeRemoveParentNodesWithNoChild();
			}

		private:
			bool CheckParentChildRelationship(TreeNode* parent, TreeNode* child)
			{
				for (auto const& item : parent->GetWhiteList()) {
					if (item.second.get() == child) {
						return true;
					}
				}
				return false;
			}

			void AdvanceNode(int choice, TreeNode* next_node)
			{
				assert(next_node);
				assert(CheckParentChildRelationship(node_, next_node));
				assert(node_->GetWhiteList()[choice].get() == next_node);

				traversed_nodes_.push_back({ node_, choice });
				node_ = next_node;
			}

			void CascadeRemoveParentNodesWithNoChild() {
				if (!node_->GetWhiteList().empty()) return;

				for (auto it = traversed_nodes_.rbegin(); it != traversed_nodes_.rend(); ++it) {
					TreeNode* parent = it->node;
					int choice = it->choice;

					auto it_child = parent->GetWhiteList().find(choice);
					assert(it_child != parent->GetWhiteList().end());
					parent->GetWhiteList().erase(it_child);
					if (!parent->GetWhiteList().empty()) break;
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