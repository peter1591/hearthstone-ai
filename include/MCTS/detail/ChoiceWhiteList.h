#pragma once

#include <assert.h>
#include <vector>
#include <unordered_map>
#include <memory>
#include <algorithm>

namespace mcts
{
	namespace detail
	{
		// Some (sub-)action results in an invalid game state
		// Ideally, we should prevent this situation as much as possible
		// However, for performance consideration and code simplicity,
		//    sometimes a (sub-)action might still yields an invalid game state
		// In this case, we want to record the path of (sub-)actions yielding an invalid state
		//    and prevent this path for a following re-trial
		class ChoiceWhiteList
		{
		private:
			class TreeNode {
			public:
				TreeNode(int parent_child_edge)
					: parent_child_edge_(parent_child_edge), choices_(0)
				{}

				void SetChoices(int choices) {
					if (choices_ > 0) {
						assert(choices_ == choices);
						return;
					}
					choices_ = choices;
					assert(white_list_.empty());
					for (int i = 0; i < choices; ++i) {
						white_list_.insert(std::make_pair(i, nullptr));
					}
				}

				auto const& GetWhiteList() const { return white_list_; }
				auto & GetWhiteList() { return white_list_; }

				int GetParentChildEdge() const { return parent_child_edge_; }

				void Clear() {
					choices_ = 0;
					white_list_.clear();
				}

			private:
				int parent_child_edge_;

				int choices_; // 0: not set
				std::unordered_map<int, std::unique_ptr<TreeNode>> white_list_;
			};

			struct PendingAction {
				int choices;
				int choice;
			};

		public:
			ChoiceWhiteList() : root_(-1), node_(&root_), last_choice_(-1)
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
						AdvanceNode(it->second.get());
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
					auto it_end = pending_actions_.end() - 1; // the last one is considered processed
					assert(node_);

					int next_edge = first_choice_before_pending_actions_;

					for (auto const& pending_action : pending_actions_) {
						auto & node_container = node_->GetWhiteList()[next_edge];
						if (!node_container.get()) {
							TreeNode* new_node = new TreeNode(next_edge);
							new_node->SetChoices(pending_action.choices);
							node_container.reset(new_node);
						}

						AdvanceNode(node_container.get());
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

			void AdvanceNode(TreeNode* node)
			{
				assert(node);
				assert(CheckParentChildRelationship(node_, node));

				traversed_nodes_.push_back(node_);
				node_ = node;;
			}

			void CascadeRemoveParentNodesWithNoChild() {
				TreeNode* processing = node_;
				while (processing->GetWhiteList().size() == 0) {
					if (traversed_nodes_.empty()) break;

					TreeNode* parent = traversed_nodes_.back();
					assert(CheckParentChildRelationship(parent, processing));
					traversed_nodes_.pop_back();

					auto it = parent->GetWhiteList().find(processing->GetParentChildEdge());
					assert(it != parent->GetWhiteList().end());
					parent->GetWhiteList().erase(it);

					processing = parent;
				}
			}

		private:
			TreeNode root_;

			TreeNode* node_;
			int last_choice_;
			std::vector<TreeNode*> traversed_nodes_;

			// The case for an invalid game state is rare
			// So we want to delay the creation of the TreeNode as late as possible
			// These fields remembers the pending, which is used to create the necessary
			//    TreeNodes when necessary.
			int first_choice_before_pending_actions_;
			std::vector<PendingAction> pending_actions_;
		};
	}
}