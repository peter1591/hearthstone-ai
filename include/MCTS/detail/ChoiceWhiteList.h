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
		class ChoiceWhiteList
		{
		private:
			class TreeNode {
			public:
				// TODO: no need parent if we record the traverse path
				TreeNode(TreeNode* parent, int parent_child_edge)
					: parent_(parent), parent_child_edge_(parent_child_edge), choices_(0)
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

				TreeNode* GetParent() const { return parent_; }
				int GetParentChildEdge() const { return parent_child_edge_; }

				void Clear() {
					choices_ = 0;
					white_list_.clear();
				}

			private:
				TreeNode* parent_;
				int parent_child_edge_;

				int choices_; // 0: not set
				std::unordered_map<int, std::unique_ptr<TreeNode>> white_list_;
			};

			struct PendingAction {
				int choices;
				int choice;
			};

		public:
			ChoiceWhiteList() : root_(nullptr, -1), node_(&root_), last_choice_(-1)
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
						node_ = it->second.get();
						node_->SetChoices(choices);
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
				TreeNode* blame_node = nullptr;
				int blame_action = 0;

				if (pending_actions_.empty()) {
					blame_node = node_;
					blame_action = last_choice_;
				}
				else {
					auto it_end = pending_actions_.end() - 1; // the last one is considered processed
					blame_action = last_choice_;
					assert(node_);

					TreeNode* parent = node_;
					int parent_child_edge = first_choice_before_pending_actions_;

					for (auto it = pending_actions_.begin(); it != it_end; ++it) {
						auto & node_container = parent->GetWhiteList()[parent_child_edge];
						if (!node_container.get()) {
							TreeNode* new_node = new TreeNode(parent, parent_child_edge);
							new_node->SetChoices(it->choices);
							node_container.reset(new_node);
						}

						parent = node_container.get();
						parent_child_edge = it->choice;
					}

					blame_node = parent;
				}

				assert(blame_node);
				auto it = blame_node->GetWhiteList().find(blame_action);
				assert(it != blame_node->GetWhiteList().end()); // one should not choose a black-listed action
				blame_node->GetWhiteList().erase(it);

				TreeNode* check_no_child_node = blame_node;
				while (check_no_child_node->GetWhiteList().size() == 0) {
					TreeNode* parent = check_no_child_node->GetParent();
					if (!parent) break;

					it = parent->GetWhiteList().find(check_no_child_node->GetParentChildEdge());
					assert(it != parent->GetWhiteList().end());
					parent->GetWhiteList().erase(it);

					check_no_child_node = parent;
				}
			}

		private:
			TreeNode root_;

			TreeNode* node_;
			int last_choice_;

			int first_choice_before_pending_actions_;
			std::vector<PendingAction> pending_actions_;
		};
	}
}