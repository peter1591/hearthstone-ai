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
	}
}