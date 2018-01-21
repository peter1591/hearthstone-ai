#pragma once

#include <string>

#include "agents/MCTSConfig.h"

namespace alphazero
{
	namespace self_play
	{
		struct RunOptions
		{
			RunOptions() :
				save_dir(),
				agent_config()
			{
				agent_config.threads = 4;
				agent_config.tree_samples = 10;
			}

			std::string save_dir;
			agents::MCTSAgentConfig agent_config;
		};
	}
}