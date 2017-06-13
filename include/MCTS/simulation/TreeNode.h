#pragma once

#include <assert.h>
#include <utility>
#include <unordered_map>

#include "MCTS/detail/NodeIndexMap.h"

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
				children_.resize(choices_);
				for (size_t i = 0; i < children_.size(); ++i) {
					// even though they are not allocated yet, they are valid nodes
					valid_idx_map_.PushBack(i);
				}
			}

			bool HasAnyChoice() const { return !valid_idx_map_.Empty(); }
			size_t GetChoiceCount() const { return valid_idx_map_.Size(); }

			// return nullptr if node is not yet allocated, or node is removed
			TreeNode* GetChoice(int choice) {
				assert(choices_ > 0); // initialized
				assert(choice >= 0 && choice < choices_);
				return children_[choice].get();
			}

			TreeNode* AllocateChoiceNode(int choice) {
				assert(choices_ > 0); // initialized
				assert(choice >= 0 && choice < choices_);
				assert(GetChoice(choice) == nullptr); // not allocated yet
				TreeNode* ptr = new TreeNode();
				children_[choice].reset(ptr);
				return ptr;
			}

			void RemoveChoice(int choice) {
				assert(choices_ > 0); // initialized
				assert(choice >= 0 && choice < choices_);
				children_[choice].release();
				valid_idx_map_.Erase(choice);
			}

			template <typename Functor>
			void ForEachWhiteListChoice(Functor&& functor) {
				assert(choices_ > 0); // initialized
				valid_idx_map_.ForEach([&](size_t valid_idx) {
					return functor((int)valid_idx, children_[valid_idx].get());
				});
			}

			void Reset() {
				choices_ = 0;
				children_.clear();
				valid_idx_map_.Clear();
			}

		private:
			int choices_; // 0: not set
			std::vector<std::unique_ptr<TreeNode>> children_;
			detail::NodeIndexMap valid_idx_map_;
		};
	}
}