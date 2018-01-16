#pragma once

namespace alphazero
{
	namespace self_play
	{
		struct RunOptions
		{
			RunOptions() :
				MCTS_iterations_per_run(1000)
			{}

			int MCTS_iterations_per_run;
		};
	}
}