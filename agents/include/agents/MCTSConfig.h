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

	public: // action policy
		double action_follow_temperature;

	public:
		MCTSAgentConfig() :
			threads(1),
			tree_samples(10),
			iterations_per_action(10000),
			callback_interval_ms(1000),
			mcts(),
			action_follow_temperature(0.0)
		{}
	};
}