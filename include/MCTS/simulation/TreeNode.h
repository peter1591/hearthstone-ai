#pragma once

#include <assert.h>
#include <utility>
#include <unordered_map>

namespace mcts
{
	namespace simulation
	{
		class TreeNode {
		public:
			TreeNode() : choices_(0) {}

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

			bool HasAnyChoice() const {
				return !white_list_.empty();
			}

			size_t GetChoiceCount() const { return white_list_.size(); }

			TreeNode* GetChoice(int choice) {
				return white_list_[choice].get();
			}

			TreeNode* AddChoice(int choice) {
				TreeNode* ptr = new TreeNode();
				assert(!white_list_[choice]);
				white_list_[choice].reset(ptr);
				return ptr;
			}

			void RemoveChoice(int choice) {
				auto it = white_list_.find(choice);
				assert(it != white_list_.end()); // one should not choose a black-listed action
				white_list_.erase(it);
			}

			template <typename Functor>
			void ForEachWhiteListChoice(Functor&& functor) {
				for (auto const& kv : white_list_) {
					int choice = kv.first;
					TreeNode* node = kv.second.get();
					if (!functor(choice, node)) return;
				}
			}

			void Clear() {
				choices_ = 0;
				white_list_.clear();
			}

		private:
			int choices_; // 0: not set
			std::unordered_map<int, std::unique_ptr<TreeNode>> white_list_; // TODO: don't use hash table
		};
	}
}