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

		public:
			ChoiceWhiteList() : root_(nullptr, -1), node_(&root_),
				blame_node_(nullptr), blame_action_(0)
			{}

			void Clear() {
				root_.Clear();
				Restart();
			}

			void Restart() {
				node_ = &root_;
				blame_node_ = nullptr;
			}

			void FillChoices(int choices) {
				assert(node_);
				assert(choices > 0);
				node_->SetChoices(choices);
			}

			size_t GetWhiteListCount() const {
				assert(node_);
				assert(node_->GetWhiteList().size() > 0);
				return node_->GetWhiteList().size();
			}
			template <typename Functor>
			void ForEachWhiteListItem(Functor&& functor) {
				assert(node_);
				for (auto it = node_->GetWhiteList().begin();
					it != node_->GetWhiteList().end();
					++it)
				{
					if (!functor(it->first)) return;
				}
			}

			void ApplyChoice(int choice) {
				assert(choice >= 0);
				blame_node_ = node_;
				blame_action_ = choice;

				StepNext(choice);
			}

			void ReportInvalidChoice() {
				assert(blame_node_);
				auto it = blame_node_->GetWhiteList().find(blame_action_);
				assert(it != blame_node_->GetWhiteList().end()); // one should not choose a black-listed action
				blame_node_->GetWhiteList().erase(it);

				TreeNode* check_no_child_node = blame_node_;
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
			void StepNext(int choice) {
				assert(choice >= 0);
				auto it = node_->GetWhiteList().find(choice);
				assert(it != node_->GetWhiteList().end()); // one should not choose a black-listed action
				if (!it->second) it->second.reset(new TreeNode(node_, choice));
				node_ = it->second.get();
			}

		private:
			TreeNode root_;
			TreeNode* node_;
			TreeNode* blame_node_;
			int blame_action_;
		};
	}
}