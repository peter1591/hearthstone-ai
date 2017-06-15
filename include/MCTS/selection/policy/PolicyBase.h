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

					template <typename Functor>
					void ForEachChoice(Functor&& functor) const
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