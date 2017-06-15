#pragma once

#include "MCTS/selection/TreeNode.h"

namespace mcts
{
	namespace selection
	{
		namespace policy
		{
			class PolicyBase
			{
			public:
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
			};
		}
	}
}