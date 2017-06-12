#pragma once

#include <assert.h>
#include <memory>
#include <vector>

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
				assert(idx < valid_children_idx_map_.size());
				size_t child_idx = valid_children_idx_map_[idx];

				assert(child_idx < children_.size());
				assert(children_[child_idx]); // child should be valid (not removed before)
				return { child_idx, children_[child_idx].get() };
			}
			template <typename Functor>
			void ForEachValidChild(Functor&& functor) const {
				for (size_t child_idx : valid_children_idx_map_) {
					assert(children_[child_idx]);
					functor((int)child_idx, children_[child_idx].get());
				}
			}
			bool HasValidChild() const { return !valid_children_idx_map_.empty(); }
			size_t GetValidChildrenCount() const { return valid_children_idx_map_.size(); }

			TreeNode* PushBackValidChild() {
				TreeNode* new_node = new TreeNode();
				valid_children_idx_map_.push_back(children_.size());
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
					for (auto it = valid_children_idx_map_.begin(); it != valid_children_idx_map_.end(); ++it)
					{
						if (*it == idx) {
							found = true;
							break;
						}
					}
					assert(found);
					return true;
				}());

				children_[idx].release();

				// A O(N) algorithm for removal (which is rare), but the good side is
				// we can have a O(1) for selection (which should be most of the cases)
				for (auto it = valid_children_idx_map_.begin(); it != valid_children_idx_map_.end(); ++it)
				{
					if (*it == idx) {
						it = valid_children_idx_map_.erase(it);
						break;
					}
				}

				// debug check
				assert([&]() {
					for (auto valid_idx : valid_children_idx_map_) {
						assert(children_[valid_idx]);
					}
					return true;
				}());
			}

			void Clear() {
				children_.clear();
				valid_children_idx_map_.clear();
			}

		private:
			std::vector<std::unique_ptr<TreeNode>> children_;

			// map the 'index to valid children' to the 'index to all (including removed) children'
			std::vector<size_t> valid_children_idx_map_;
		};
	}
}