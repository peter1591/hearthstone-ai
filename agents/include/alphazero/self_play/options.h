#pragma once

#include "agents/MCTSConfig.h"

namespace alphazero
{
	namespace self_play
	{
		struct RunOptions
		{
			RunOptions() :
				agent_config()
			{
				agent_config.iterations_per_action = 100;
				agent_config.threads = 1;
				agent_config.tree_samples = 10;
			}

			agents::MCTSAgentConfig agent_config;
		};
	}
}