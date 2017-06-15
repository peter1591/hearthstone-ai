#pragma once

#include <assert.h>
#include <memory>
#include <vector>

#include "MCTS/detail/NodeIndexMap.h"

namespace mcts
{
	namespace detail
	{
		// Maintain two groups of children:
		//    (1) (Normal) children
		//    (2) Valid children
		// Goal:
		//    Quickly random-access to (1) and (2)
		//    The mark-invalid operation can be slower since it should happen rarely
		template <typename TreeNode>
		class TreeNodeBase
		{
		public:
			// @parameter origin_idx  Index among all children
			TreeNode* GetChild(size_t idx) {
				assert(idx >= 0);
				assert(idx < children_.size());
				TreeNode* child = children_[idx].get();
				assert(child); // child should not be a nullptr (i.e., a removed child)
				return child;
			}
			bool HasChild() const { return !children_.empty(); }
			size_t GetChildrenCount() const { return children_.size(); }

			// @parameter idx  The index among valid children
			// @rerturn  The original index when added, and a pointer to that node
			std::pair<size_t, TreeNode*> GetValidChild(size_t idx) const {
				size_t child_idx = valid_idx_map_.Get(idx);

				assert(child_idx < children_.size());
				assert(children_[child_idx]); // child should be valid (not removed before)
				return { child_idx, children_[child_idx].get() };
			}
			template <typename Functor>
			void ForEachValidChild(Functor&& functor) const {
				valid_idx_map_.ForEach([&](size_t child_idx) {
					assert(children_[child_idx]);
					return functor((int)child_idx, children_[child_idx].get());
				});
			}
			bool HasValidChild() const { return !valid_idx_map_.Empty(); }
			size_t GetValidChildrenCount() const { return valid_idx_map_.Size(); }

			TreeNode* PushBackValidChild() {
				TreeNode* new_node = new TreeNode();
				valid_idx_map_.PushBack(children_.size());
				children_.push_back(std::unique_ptr<TreeNode>(new_node));
				return new_node;
			}

			void PushBackInvalidChild(int count = 1) {
				size_t new_size = children_.size() + (size_t)count;
				children_.resize(new_size);
			}

			void MarkInvalid(size_t idx) {
				assert(idx < children_.size());
				assert(children_[idx]); // not marked as invalid before

				assert([&]() {
					bool found = false;
					valid_idx_map_.ForEach([&](size_t idx2) {
						if (idx == idx2) {
							found = true;
							return false;
						}
						return true;
					});
					assert(found);
					return true;
				}());

				children_[idx].release();

				// A O(N) algorithm for removal (which is rare), but the good side is
				// we can have a O(1) for selection (which should be most of the cases)
				valid_idx_map_.Erase(idx);

				// check all valid indeics are actually valid
				assert([&]() {
					valid_idx_map_.ForEach([&](size_t idx) {
						assert(children_[idx]);
						return true;
					});
					return true;
				}());
			}

			void Clear() {
				children_.clear();
				valid_idx_map_.Clear();
			}

		private:
			std::vector<std::unique_ptr<TreeNode>> children_;
			NodeIndexMap valid_idx_map_;
		};
	}
}