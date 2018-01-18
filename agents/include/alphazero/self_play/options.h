#pragma once

#include "agents/MCTSConfig.h"

namespace alphazero
{
	namespace self_play
	{
		struct RunOptions
		{
			RunOptions() :
				MCTS_iterations_per_run(1000),
				agent_config()
			{
				agent_config.iterations_per_action = 10000;
				agent_config.threads = 1;
				agent_config.tree_samples = 10;
			}

			int MCTS_iterations_per_run;
			agents::MCTSAgentConfig agent_config;
		};
	}
}