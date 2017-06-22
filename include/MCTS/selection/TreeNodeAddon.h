#pragma once

#include "MCTS/detail/SOMCTS_TreeNodeAddon.h"

namespace mcts
{
	namespace selection
	{
		// Add abilities to tree node to use in SO-MCTS
		// Note: this will lived in *every* tree node, so careful about memory footprints
		struct TreeNodeAddon
		{
			detail::SOMCTS_TreeNodeAddon somcts;
		};
	}
}