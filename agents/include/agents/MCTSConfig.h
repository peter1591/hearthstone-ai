#pragma once

#include "MCTS/Config.h"

namespace agents
{
	class MCTSAgentConfig {
	public:
		int threads;
		int tree_samples;

		int iterations_per_action;

		mcts::Config mcts;
	};
}