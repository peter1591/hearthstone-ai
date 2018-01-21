#pragma once

#include "MCTS/Config.h"

namespace agents
{
	class MCTSAgentConfig {
	public:
		int threads;
		int tree_samples;

		int iterations_per_action;
		int callback_interval_ms;

		mcts::Config mcts;

		MCTSAgentConfig() :
			threads(1),
			tree_samples(10),
			iterations_per_action(10000),
			callback_interval_ms(1000),
			mcts()
		{}
	};
}