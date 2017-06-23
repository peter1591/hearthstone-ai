#pragma once

#include "MCTS/selection/TreeNode.h"
#include "MCTS/board/Board.h"

namespace mcts
{
	namespace policy
	{
		namespace selection
		{
			using TreeNode = mcts::selection::TreeNode;

			class ChoiceGetter
			{
			public:
				ChoiceGetter(TreeNode const& node) : node_(node) {}

				size_t Size() const { return node_.GetChildrenCount(); }

				std::pair<int, TreeNode*> Get(size_t idx) const {
					return node_.GetNthChild(idx);
				}

				template <typename Functor>
				void ForEach(Functor&& functor) const
				{
					return node_.ForEachChild(std::forward<Functor>(functor));
				}

			private:
				TreeNode const& node_;
			};

			class RandomPolicy
			{
			public:
				static std::pair<int, TreeNode*>
					GetChoice(ChoiceGetter const& choice_getter, board::Board const& board)
				{
					size_t count = choice_getter.Size();
					size_t idx = 0;
					size_t rand_idx = (size_t)(std::rand() % count);
					std::pair<int, TreeNode*> result;
					choice_getter.ForEach([&](int action, TreeNode* child) mutable {
						if (idx == rand_idx) {
							result = std::make_pair(action, child);
							return false;
						}
						++idx;
						return true;
					});
					std::pair<int, TreeNode*> result2 = choice_getter.Get(rand_idx);
					assert(result == result2);
					return result;
				}
			};
		};
	}
}