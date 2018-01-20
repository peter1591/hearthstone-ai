#pragma once

namespace alphazero
{
	namespace evaluation
	{
		struct RunOptions
		{
			RunOptions() :
				runs(1000),
				show_interval_ms(1000),
				agent_config()
			{}

			int runs;
			int show_interval_ms;
			agents::MCTSAgentConfig agent_config;
		};
	}
}