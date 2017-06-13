#pragma once

#include <assert.h>
#include <utility>
#include <unordered_map>

namespace mcts
{
	namespace simulation
	{
		// The tree node is delay-allocated
		// If it's nullptr, it's a valid (child) node, but not yet being allocated
		class TreeNode {
		public:
			TreeNode() : choices_(0) {}

 			// The newly-created choices are delayed-mallocated
			// So a following GetChoice() returns a nullptr
			void Initialize(int choices) {
				assert(choices > 0);
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

			bool HasAnyChoice() const { return !white_list_.empty(); }
			size_t GetChoiceCount() const { return white_list_.size(); }

			// return nullptr if node is not yet allocated
			TreeNode* GetChoice(int choice) {
				assert(choices_ > 0); // initialized
				assert(choice >= 0 && choice < choices_);
				return white_list_[choice].get();
			}

			TreeNode* AllocateChoiceNode(int choice) {
				assert(choices_ > 0); // initialized
				assert(choice >= 0 && choice < choices_);
				assert(GetChoice(choice) == nullptr); // not allocated yet
				TreeNode* ptr = new TreeNode();
				white_list_[choice].reset(ptr);
				return ptr;
			}

			void RemoveChoice(int choice) {
				assert(choices_ > 0); // initialized
				assert(choice >= 0 && choice < choices_);
				white_list_.erase(choice);
			}

			template <typename Functor>
			void ForEachWhiteListChoice(Functor&& functor) {
				assert(choices_ > 0); // initialized
				for (auto const& kv : white_list_) {
					int choice = kv.first;
					TreeNode* node = kv.second.get();
					if (!functor(choice, node)) return;
				}
			}

			void Reset() {
				choices_ = 0;
				white_list_.clear();
			}

		private:
			int choices_; // 0: not set
			std::unordered_map<int, std::unique_ptr<TreeNode>> white_list_; // TODO: don't use hash table
		};
	}
}